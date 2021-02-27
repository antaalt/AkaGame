#include "CameraSystem.h"

#include <Aka/Scene/World.h>
#include <Aka/Platform/Input.h>
#include <Aka/OS/Logger.h>

#include "../Component/Camera2D.h"
#include "../Component/Player.h"
#include "../Component/Transform2D.h"
#include "../Component/TileLayer.h"


namespace aka {
CameraSystem::CameraSystem(WorldMap& map) :
	m_map(map)
{
}
void CameraSystem::update(World& world, Time::Unit deltaTime)
{
	// Shake screen when hit receive
	/*m_world->receive<CollisionEvent>([](CollisionEvent* collision) {

	});*/
	auto cameraView = world.registry().view<Camera2D>();
	auto playerView = world.registry().view<Player, Transform2D>();
	auto layerView = world.registry().view<TileLayer>();
	for (entt::entity cameraEntity : cameraView)
	{
		Transform2D& cameraTransform = world.registry().get<Transform2D>(cameraEntity);
		Camera2D& cameraComponent = world.registry().get<Camera2D>(cameraEntity);
		if (!cameraComponent.main || !cameraComponent.tracking)
			continue;
		CameraOrthographic& camera = cameraComponent.camera;
		// Track the player
		for (entt::entity playerEntity : playerView)
		{
			Player& player = world.registry().get<Player>(playerEntity);
			Transform2D& transform = world.registry().get<Transform2D>(playerEntity);

			const float hThreshold = 0.4f * camera.viewport.x;
			const float vThreshold = 0.2f * camera.viewport.y;
			const vec2f playerPosition = transform.position;
			const vec2f playerRelativePosition = playerPosition - cameraTransform.position;
			Level& level = m_map.get();
			if (level.size.x > level.size.y)
			{
				// Horizontal level
				if (playerRelativePosition.x < hThreshold)
				{
					float distance = abs<float>(playerRelativePosition.x - hThreshold);
					cameraTransform.position.x = cameraTransform.position.x - distance;
				}
				else if (playerRelativePosition.x > camera.viewport.x - hThreshold)
				{
					float distance = playerRelativePosition.x - (camera.viewport.x - hThreshold);
					cameraTransform.position.x = cameraTransform.position.x + distance;
				}
			}
			else
			{
				// Vertical level
				if (playerRelativePosition.y < vThreshold)
				{
					float distance = abs<float>(playerRelativePosition.y - vThreshold);
					cameraTransform.position.y = cameraTransform.position.y - distance;
				}
				else if (playerRelativePosition.y > camera.viewport.y - vThreshold)
				{
					float distance = playerRelativePosition.y - (camera.viewport.y - vThreshold);
					cameraTransform.position.y = cameraTransform.position.y + distance;
				}
			}
			break; // Only track first player encountered.
		}

		// Clamp camera position to the current level bounds.
		if (cameraComponent.clampBorder)
		{
			Level& level = m_map.get();
			vec2f grid = vec2f(level.size) + vec2f(level.offset);
			cameraTransform.position.x = max(cameraTransform.position.x, (float)level.offset.x);
			cameraTransform.position.x = min(cameraTransform.position.x, grid.x - camera.viewport.x);
			cameraTransform.position.y = max(cameraTransform.position.y, (float)level.offset.y);
			cameraTransform.position.y = min(cameraTransform.position.y, grid.y - camera.viewport.y);
		}
	}
}

}