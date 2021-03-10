#pragma once

#include <Aka/Scene/Component.h>
#include <Aka/Core/Geometry.h>

namespace aka {

struct Transform2D
{
	Transform2D();
	Transform2D(const vec2f& position, const vec2f& scale, radianf rotation);

	vec2f position;
	vec2f size;
	radianf rotation;

	mat3f model() const;
};

}