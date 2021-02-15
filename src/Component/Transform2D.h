#pragma once

#include <Aka/Scene/Component.h>
#include <Aka/Core/Geometry.h>

namespace aka {

struct Transform2D
{
	Transform2D();
	Transform2D(const vec2f& position, const vec2f& scale, radianf rotation);

	mat3f model;

	vec2f position() const;
	vec2f size() const;

	void translate(const vec2f& translation);
	void move(const vec2f& move);
	void rotate(radianf rotation);
	void scale(const vec2f& scale);
};
template <>
const char* ComponentHandle<Transform2D>::name = "Transform2D";

}