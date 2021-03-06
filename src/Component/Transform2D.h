#pragma once

#include <Aka/Scene/Component.h>
#include <Aka/Core/Geometry.h>

namespace aka {

struct Transform2DComponent
{
	Transform2DComponent();
	Transform2DComponent(const vec2f& position, const vec2f& scale, anglef rotation);

	vec2f position;
	vec2f size;
	vec2f pivot;
	anglef rotation;

	mat3f model() const;
};

}