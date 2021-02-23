#include "Transform2D.h"

namespace aka {

Transform2D::Transform2D() :
	Transform2D(vec2f(0.f), vec2f(1.f), radianf(0.f))
{
}

Transform2D::Transform2D(const vec2f& position, const vec2f& scale, radianf rotation) :
	position(position),
	size(scale),
	rotation(rotation)
{
}

mat3f Transform2D::model() const
{
	return mat3f::translate(position) * mat3f::rotate(rotation) * mat3f::scale(size);
	/*mat3f model = mat4f::identity();
	// Translate
	model *= mat3f::translate(position);
	// Rotate around center of object
	model *= mat3f::translate(vec2f(0.5f * size.x, 0.5f * size.y));
	model *= mat3f::rotate(rotation);
	model *= mat3f::translate(vec2f(-0.5f * size.x, -0.5f * size.y));
	// Scale
	model *= mat3f::scale(size);
	return model;*/
}

};