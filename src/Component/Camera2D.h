#pragma once

#include <Aka/Core/Geometry.h>
#include <Aka/Scene/Component.h>
#include <Aka/Scene/Camera.h>

namespace aka {

struct Camera2DComponent
{
	Camera2DComponent() : Camera2DComponent(vec2f(1.f)) {}
	Camera2DComponent(const vec2f& viewport) : camera{}, main(false) {
		camera.left = camera.bottom = 0.f;
		camera.right = viewport.x;
		camera.top = viewport.y;
		camera.nearZ = -1.f;
		camera.farZ = 1.f;
	}

	CameraOrthographic camera;
	bool main; // Is it the main camera in the scene ?
};

// TODO handle this in world directly.
inline Entity getMainCamera2DEntity(World& world)
{
	auto view = world.registry().view<Camera2DComponent>();
	for (entt::entity cameraEntity : view)
		if (world.registry().get<Camera2DComponent>(cameraEntity).main)
			return Entity(cameraEntity, &world);
	throw std::runtime_error("No main camera in scene");
}

}

