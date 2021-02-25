#pragma once

#include <Aka/Core/Geometry.h>
#include <Aka/Scene/Component.h>
#include <Aka/Scene/Camera.h>

namespace aka {

struct Camera2D
{
	Camera2D() : Camera2D(vec2f(1.f)) {}
	Camera2D(const vec2f& viewport) : camera(viewport), main(false), clampBorder(true) {}

	CameraOrthographic camera;
	bool main;
	bool clampBorder;
};
template <>
const char* ComponentHandle<Camera2D>::name = "Camera2D";

}

