#pragma once

#include <Aka/Core/Geometry.h>
#include <Aka/Scene/Component.h>

namespace aka {

struct Camera2D
{
	Camera2D();
	Camera2D(const vec2f position, const vec2f& viewport);

	vec2f position;
	vec2f viewport;
};

}

