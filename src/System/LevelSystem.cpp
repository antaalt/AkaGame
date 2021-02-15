#include "LevelSystem.h"

#include <Aka/Core/Geometry.h>

#include "../Component/Player.h"
#include "../Component/Transform2D.h"
#include "../Component/Collider2D.h"

namespace aka {

LevelSystem::LevelSystem(WorldMap& map) :
	m_map(map)
{
}

void LevelSystem::update(World& world, Time::Unit deltaTime)
{
	// TODO handle transition.
	Level& level = m_map.get();
	auto view = world.registry().view<Player, Collider2D, Transform2D>();
	view.each([&](Player& player, Collider2D& collider, Transform2D& transform) {
		vec2f pos = transform.model.multiplyPoint(collider.position);
		vec2f size = transform.model.multiplyVector(collider.size);

		int32_t xOffset = 0;
		int32_t yOffset = 0;
		if (pos.x + size.x > level.size.x + level.offset.x)
		{
			xOffset = 1;
			Logger::info("Next level x");
		}
		else if (pos.x + size.x < level.offset.x)
		{
			xOffset = -1;
			Logger::info("Previous level x");
		}
		else if (pos.y + size.y > level.size.y + level.offset.y)
		{
			yOffset = 1;
			Logger::info("Next level y");
		}
		else if (pos.y + size.y < level.offset.y)
		{
			yOffset = -1;
			Logger::info("Previous level y");
		}
		if(xOffset != 0 || yOffset != 0)
			m_map.next(xOffset, yOffset, world);
	});
}

};