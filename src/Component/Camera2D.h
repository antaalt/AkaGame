#pragma once

#include <Aka/Core/Geometry.h>
#include <Aka/Scene/Component.h>

namespace aka {

struct Camera2D
{
	Camera2D() : Camera2D(vec2f(0.f), vec2f(1.f)) {}
	Camera2D(const vec2f position, const vec2f& viewport) : position(position), viewport(viewport) {}

	vec2f position;
	vec2f viewport;
};
template <>
const char* ComponentHandle<Camera2D>::name = "Camera2D";

}

