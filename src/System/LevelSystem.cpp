#include "LevelSystem.h"

#include <Aka/Core/Geometry.h>

#include "../Component/Player.h"
#include "../Component/Transform2D.h"
#include "../Component/Collider2D.h"
#include "../Component/Camera2D.h"

namespace aka {

LevelSystem::LevelSystem(WorldMap& map) :
	m_transition(false),
	m_map(map)
{
}

void LevelSystem::update(World& world, Time::Unit deltaTime)
{
	static Time::Unit transitionStartedTime = Time::zero();
	static vec2f transitionStartPosition;
	static vec2f transitionEndPosition;
	static vec2u previous;

	Entity cameraEntity = getMainCamera2DEntity(world);
	Level& level = m_map.get();
	
	if (!m_transition)
	{
		auto view = world.registry().view<Player, Collider2D, Transform2D>();
		view.each([&](Player& player, Collider2D& collider, Transform2D& transform) {
			vec2f pos = transform.model().multiplyPoint(collider.position);
			vec2f size = transform.model().multiplyVector(collider.size);

			int32_t xOffset = 0;
			int32_t yOffset = 0;
			if (pos.x + size.x > level.size.x + level.offset.x)
			{
				xOffset = 1;
				transitionStartPosition = cameraEntity.get<Transform2D>().position;
				transitionEndPosition = vec2f(
					(float)(level.size.x + level.offset.x),
					transitionStartPosition.y
				);
				Logger::info("Next level x");
			}
			else if (pos.x + size.x < level.offset.x)
			{
				xOffset = -1;
				transitionStartPosition = cameraEntity.get<Transform2D>().position;
				transitionEndPosition = vec2f(
					(float)(level.offset.x - cameraEntity.get<Camera2D>().camera.viewport.x),
					transitionStartPosition.y
				);
				Logger::info("Previous level x");
			}
			else if (pos.y + size.y > level.size.y + level.offset.y)
			{
				yOffset = 1;
				transitionStartPosition = cameraEntity.get<Transform2D>().position;
				transitionEndPosition = vec2f(
					transitionStartPosition.x,
					(float)(level.size.y + level.offset.y)
				);
				Logger::info("Next level y");
			}
			else if (pos.y + size.y < level.offset.y)
			{
				yOffset = -1;
				transitionStartPosition = cameraEntity.get<Transform2D>().position;
				transitionEndPosition = vec2f(
					transitionStartPosition.x,
					(float)(level.offset.y - cameraEntity.get<Camera2D>().camera.viewport.y)
				);
				Logger::info("Previous level y");
			}
			if (xOffset != 0 || yOffset != 0)
			{
				m_transition = true;
				player.controllable = false;
				previous = m_map.current();
				m_map.next(xOffset, yOffset, world);
				transitionStartedTime = Time::now();
				// TODO add disabled component ?
			}
		});
	}
	else
	{
		Transform2D& cameraTransform = cameraEntity.get<Transform2D>();
		Camera2D& cameraComponent = cameraEntity.get<Camera2D>();
		if (!cameraComponent.clampBorder)
		{
			// end transition immediately & reenable player
			auto view = world.registry().view<Player>();
			view.each([&](Player& player) {
				player.controllable = true;
			});
			m_transition = false;
		}
		else
		{
			cameraComponent.tracking = false;
			const Time::Unit transitionDuration = Time::Unit::milliseconds(1000);
			float t = (Time::now() - transitionStartedTime).seconds() / transitionDuration.seconds();
			if (t > 1.f)
			{
				// Transition finished
				auto view = world.registry().view<Player>();
				view.each([&](Player& player) {
					player.controllable = true;
				});
				cameraComponent.tracking = true;
				m_transition = false;
				m_map.destroy(previous.x, previous.y, world);
			}
			else
			{
				// Manage camera position
				cameraTransform.position = lerp(transitionStartPosition, transitionEndPosition, t);
			}
		}
	}
}

};