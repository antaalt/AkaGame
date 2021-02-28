#include "PlayerSystem.h"

#include <Aka/OS/Logger.h>
#include <Aka/Scene/World.h>
#include "../Component/Transform2D.h"
#include "../Component/Coin.h"
#include "../Component/Player.h"
#include "../Component/Animator.h"
#include "../Component/Text.h"
#include "../Component/Door.h"
#include "../Component/SoundInstance.h"
#include "../Game/Resources.h"

namespace aka {

void PlayerSystem::create(World& world)
{
	world.dispatcher().sink<CollisionEvent>().connect<&PlayerSystem::receive>(*this);
}

void PlayerSystem::destroy(World& world)
{
	world.dispatcher().sink<CollisionEvent>().disconnect<&PlayerSystem::receive>(*this);
}

void PlayerSystem::receive(const CollisionEvent& event)
{
	Entity playerEntity = event.dynamicEntity;
	Entity staticEntity = event.staticEntity;
	if (!playerEntity.valid() || !staticEntity.valid() || !playerEntity.has<Player>())
		return;
	Player& player = playerEntity.get<Player>();
	if (staticEntity.has<Coin>())
	{
		Coin& coin = staticEntity.get<Coin>();
		if (!coin.picked)
		{
			coin.picked = true;
			player.coin++;
			if (staticEntity.has<Animator>())
				staticEntity.get<Animator>().play("Picked");
			if (playerEntity.has<Text>())
				playerEntity.get<Text>().text = std::to_string(player.coin);
		}
	}
	else if (CollisionFace::Bottom == event.face)
	{
		player.state = Player::State::Idle;
		//playerEntity.get<Animator>().play("Idle");
	}
}

void PlayerSystem::update(World& world, Time::Unit deltaTime)
{

	world.dispatcher().update<CollisionEvent>();
	auto view = world.registry().view<Player, Transform2D, RigidBody2D, Animator>();
	view.each([&](Player& player, Transform2D& transform, RigidBody2D& rigid, Animator& animator) {

		// Run
		const float runSpeed = player.speed.metric();
		const float runFriction = 30.f;
		// Jump
		const float initialJumpVelocity = 16.f;
		const float initialDoubleJumpVelocity = 12.f;
		const float jumpLateralAcceleration = 20.f;
		const float jumpLateralFriction = 10.f;
		const float maxJumpLateralAcceleration = 10.f;

		if (!player.controllable)
		{
			animator.play("Idle");
			rigid.velocity = vec2f(0.f);
			return;
		}
		if (player.state == Player::State::Jumping || player.state == Player::State::DoubleJumping)
		{
			if (input::pressed(player.left))
			{
				if (input::down(player.left))
					animator.play("Run");
				animator.flipU = true;
				rigid.velocity.x -= jumpLateralAcceleration * deltaTime.seconds();
				rigid.velocity.x = clamp(rigid.velocity.x, -maxJumpLateralAcceleration, maxJumpLateralAcceleration);
			}
			else if (input::pressed(player.right))
			{
				if (input::down(player.right))
					animator.play("Run");
				animator.flipU = false;
				rigid.velocity.x += jumpLateralAcceleration * deltaTime.seconds();
				rigid.velocity.x = clamp(rigid.velocity.x, -maxJumpLateralAcceleration, maxJumpLateralAcceleration);
			}
			else if (rigid.velocity.x != 0.f)
			{
				// slow down until idle.
				if (rigid.velocity.x > 0.f)
					rigid.velocity.x = max(rigid.velocity.x - jumpLateralFriction * deltaTime.seconds(), 0.f);
				else
					rigid.velocity.x = min(rigid.velocity.x + jumpLateralFriction * deltaTime.seconds(), 0.f);
			}

			if (input::down(player.jump) && player.state == Player::State::Jumping)
			{
				world.createEntity("DoubleJumpFX").add<SoundInstance>(SoundInstance(AudioManager::get("Jump"), 1.f));
				player.state = Player::State::DoubleJumping;
				rigid.velocity.y = initialDoubleJumpVelocity;
				// Allow double jumping in the opposite direction.
				if (input::pressed(player.left) && rigid.velocity.x > 0.f)
				{
					rigid.velocity.x = -initialDoubleJumpVelocity;
				}
				else if (input::pressed(player.right) && rigid.velocity.x < 0.f)
				{
					rigid.velocity.x = initialDoubleJumpVelocity;
				}
			}
		}
		else
		{
			if (input::pressed(player.left))
			{
				if (input::down(player.left))
					animator.play("Run");
				animator.flipU = true;
				player.state = Player::State::Walking;
				rigid.velocity.x = -runSpeed;
			}
			else if (input::pressed(player.right))
			{
				if (input::down(player.right))
					animator.play("Run");
				animator.flipU = false;
				player.state = Player::State::Walking;
				rigid.velocity.x = runSpeed;
			}
			else if (rigid.velocity.x != 0.f)
			{
				// slow down until idle.
				if (rigid.velocity.x > 0.f)
					rigid.velocity.x = max(rigid.velocity.x - runFriction * deltaTime.seconds(), 0.f);
				else
					rigid.velocity.x = min(rigid.velocity.x + runFriction * deltaTime.seconds(), 0.f);
			}
			else if (animator.getCurrentSpriteAnimation().name != "Idle")
			{
				animator.play("Idle");
				player.state = Player::State::Idle;
			}

			if (input::down(player.jump))
			{
 				world.createEntity("JumpFX").add<SoundInstance>(SoundInstance(AudioManager::get("Jump"), 1.f));
				player.state = Player::State::Jumping;
				rigid.velocity.y = initialJumpVelocity;
			}
		}
	});
}

};