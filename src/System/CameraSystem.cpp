#include "CameraSystem.h"

#include <Aka/Scene/World.h>
#include <Aka/Platform/Input.h>
#include <Aka/OS/Logger.h>

#include "../Component/Camera2D.h"
#include "../Component/Player.h"
#include "../Component/Transform2D.h"
#include "../Component/TileLayer.h"


namespace aka {

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
		Camera2D &camera = world.registry().get<Camera2D>(cameraEntity);
		// Track the player
		for (entt::entity playerEntity : playerView)
		{
			Player& player = world.registry().get<Player>(playerEntity);
			Transform2D& transform = world.registry().get<Transform2D>(playerEntity);

			const float hThreshold = 0.4f * camera.viewport.x;
			const float vThreshold = 0.2f * camera.viewport.y;
			const vec2f playerPosition = transform.position;
			const vec2f playerRelativePosition = playerPosition - camera.position;
			// Horizontal
			if (playerRelativePosition.x < hThreshold)
			{
				float distance = abs<float>(playerRelativePosition.x - hThreshold);
				if (distance > 1.f)
					camera.position.x -= pow<float>(distance * 10.f * deltaTime.seconds(), 2.f);
			}
			else if (playerRelativePosition.x > camera.viewport.x - hThreshold)
			{
				float distance = playerRelativePosition.x - (camera.viewport.x - hThreshold);
				if (distance > 1.f)
					camera.position.x += pow<float>(distance * 10.f * deltaTime.seconds(), 2.f);
			}

			// Vertical
			/*if (playerRelativePosition.y < vThreshold)
			{
				float distance = abs<float>(playerRelativePosition.y - vThreshold);
				if (distance > 1.f)
					camera.position.y -= pow<float>(distance * 10.f * deltaTime.seconds(), 2.f);
			}
			else if (playerRelativePosition.y > camera.viewport.y - vThreshold)
			{
				float distance = playerRelativePosition.y - (camera.viewport.y - vThreshold);
				if (distance > 1.f)
					camera.position.y += pow<float>(distance * 10.f * deltaTime.seconds(), 2.f);
			}*/
			break; // Only track first player encountered.
		}

		// Clamp camera position to the current level bounds.
		if (camera.clampBorder)
		{
			for (entt::entity layerEntity : layerView)
			{
				TileLayer& layer = world.registry().get<TileLayer>(layerEntity);
				vec2f grid = vec2f(layer.gridSize * layer.gridCount) + layer.offset;
				camera.position.x = max<float>(camera.position.x, layer.offset.x);
				camera.position.x = min<float>(camera.position.x, grid.x - camera.viewport.x);
				camera.position.y = max<float>(camera.position.y, layer.offset.y);
				camera.position.y = min<float>(camera.position.y, grid.y - camera.viewport.y);
				break; // Only first layer encountered
			}
		}
	}
}

}