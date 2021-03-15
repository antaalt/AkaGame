#include "Transform2D.h"

namespace aka {

Transform2DComponent::Transform2DComponent() :
	Transform2DComponent(vec2f(0.f), vec2f(1.f), radianf(0.f))
{
}

Transform2DComponent::Transform2DComponent(const vec2f& position, const vec2f& scale, radianf rotation) :
	position(position),
	size(scale),
	pivot(0.f),
	rotation(rotation)
{
}

mat3f Transform2DComponent::model() const
{
	mat3f model = mat4f::identity();
	// Translate
	model *= mat3f::translate(position);
	// Rotate around center of object
	model *= mat3f::translate(pivot);
	model *= mat3f::rotate(rotation);
	model *= mat3f::translate(-pivot);
	// Scale
	model *= mat3f::scale(size);
	return model;
}

};