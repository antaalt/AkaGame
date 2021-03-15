#pragma once

//#include "Shape2D.h"
#include <Aka/Core/Geometry.h>
#include <Aka/Scene/Component.h>

namespace aka {

enum class CollisionType
{
	Solid = 1,
	Event = 2,
};

struct Collider2DComponent
{
	Collider2DComponent() : Collider2DComponent(vec2f(0), vec2f(1)) {}
	Collider2DComponent(const vec2f& position, const vec2f& size, CollisionType type = CollisionType::Solid, float bouncing = 0.f, float friction = 0.f) :
		type(type), position(position), size(size), bouncing(bouncing), friction(friction) {}

	bool is(CollisionType c) { return (((int)type & (int)c) == (int)c); }


	CollisionType type;
	vec2f position;
	vec2f size;

	float bouncing;
	float friction;
};

struct RigidBody2DComponent
{
	RigidBody2DComponent() : RigidBody2DComponent(1.f) {}
	RigidBody2DComponent(float mass) : acceleration(0.f), velocity(0.f), mass(mass) {}

	vec2f acceleration; // m/s^2
	vec2f velocity; // m/s
	float mass; // kg
};

}