#include "PlayerSystem.h"

#include <Aka/OS/Logger.h>
#include <Aka/Scene/World.h>
#include "../Component/Transform2D.h"
#include "../Component/Coin.h"
#include "../Component/Player.h"
#include "../Component/Animator.h"
#include "../Component/Text.h"
#include "../Component/Door.h"
#include "SoundSystem.h"
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
	else
	{
		player.state = Player::State::Idle;
	}
}

void PlayerSystem::update(World& world, Time::Unit deltaTime)
{
	world.dispatcher().update<CollisionEvent>();
	auto view = world.registry().view<Player, Transform2D, RigidBody2D, Animator>();
	view.each([&](Player& player, Transform2D& transform, RigidBody2D& rigid, Animator& animator) {
		player.jump.update(deltaTime);
		player.left.update(deltaTime);
		player.right.update(deltaTime);

		if (player.state == Player::State::Jumping || player.state == Player::State::DoubleJumping)
		{
			if (player.left.pressed())
			{
				if (player.left.down())
					animator.play("Run");
				animator.flipU = true;
				rigid.velocity.x = -player.speed.metric();
			}
			else if (player.right.pressed())
			{
				if (player.right.down())
					animator.play("Run");
				animator.flipU = false;
				rigid.velocity.x = player.speed.metric();
			}
			else if (player.right.up() || player.left.up())
			{
				animator.play("Idle");
				rigid.velocity = vec2f(0.f);
			}

			if (player.jump.down() && player.state == Player::State::Jumping)
			{
				world.createEntity("DoubleJumpFX").add<SoundInstance>(SoundInstance(*Resources::audio.get("Jump"), 1.f));
				player.state = Player::State::DoubleJumping;
				rigid.acceleration.y = 0.f;
				rigid.velocity.y = 16.f;
			}
		}
		else
		{
			if (player.left.pressed())
			{
				if (player.left.down())
					animator.play("Run");
				animator.flipU = true;
				player.state = Player::State::Walking;
				rigid.velocity.x = -player.speed.metric();
			}
			else if (player.right.pressed())
			{
				if (player.right.down())
					animator.play("Run");
				animator.flipU = false;
				player.state = Player::State::Walking;
				rigid.velocity.x = player.speed.metric();
			}
			else if (player.right.up() || player.left.up())
			{
				animator.play("Idle");
				player.state = Player::State::Idle;
				rigid.velocity = vec2f(0.f);
			}

			if (player.jump.down())
			{
 				world.createEntity("JumpFX").add<SoundInstance>(SoundInstance(*Resources::audio.get("Jump"), 1.f));
				player.state = Player::State::Jumping;
				rigid.velocity.y = 16.f;
			}
		}
		/*switch (player.state)
		{
		case Player::State::Idle:
			Logger::info("Idle");
			break;
		case Player::State::Walking:
			Logger::info("Walking");
			break;
		case Player::State::Falling:
			Logger::info("Falling");
			break;
		case Player::State::Jumping:
			Logger::info("Jumping");
			break;
		case Player::State::DoubleJumping:
			Logger::info("DoubleJumping");
			break;
		}*/

		if (input::pressed(input::Key::LeftCtrl))
		{
			transform.move(vec2f(80, 224));
		}
	});
}

};