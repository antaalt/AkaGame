#pragma once

#include <Aka/Core/Geometry.h>
#include <Aka/Scene/Component.h>
#include <Aka/Scene/Camera.h>

namespace aka {

struct Camera2D
{
	Camera2D() : Camera2D(vec2f(1.f)) {}
	Camera2D(const vec2f& viewport) : camera(viewport), main(false), tracking(true), clampBorder(true) {}

	CameraOrthographic camera;
	bool main; // Is it the main camera in the scene ?
	bool tracking; // Is the camera tracking player ?
	bool clampBorder; // Do we prevent camera from exiting level bound ?
};

// TODO handle this in world directly.
inline Entity getMainCamera2DEntity(World& world)
{
	auto view = world.registry().view<Camera2D>();
	for (entt::entity cameraEntity : view)
		if (world.registry().get<Camera2D>(cameraEntity).main)
			return Entity(cameraEntity, &world);
	throw std::runtime_error("No main camera in scene");
}

template <>
const char* ComponentHandle<Camera2D>::name = "Camera2D";

}

