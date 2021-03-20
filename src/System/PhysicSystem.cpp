#include "PhysicSystem.h"

#include <Aka/OS/Time.h>
#include <Aka/Core/Geometry.h>
#include <Aka/Scene/World.h>
#include "../Component/Transform2D.h"
#include "../Component/Collider2D.h"

namespace aka {

static vec2f maxVelocity(50.f);

void PhysicSystem::update(World& world, Time::Unit deltaTime)
{
	const vec2f force = vec2f(0.f, -9.81f) + vec2f(0.f, 2.f); // gravity + air resistance
	float dt = deltaTime.seconds();
	auto view = world.registry().view<RigidBody2DComponent>();
	view.each([dt, force](RigidBody2DComponent& rigid) {
		vec2f acceleration = (force / rigid.mass); // F=ma, acceleration is in m/s^2
		rigid.velocity += acceleration * dt; // m/s
		rigid.velocity.x = clamp(rigid.velocity.x, -maxVelocity.x, maxVelocity.x);
		rigid.velocity.y = clamp(rigid.velocity.y, -maxVelocity.y, maxVelocity.y);
	});
	// Move all rigid body.
	auto viewTransform = world.registry().view<RigidBody2DComponent, Transform2DComponent>();
	viewTransform.each([dt](RigidBody2DComponent& rigid, Transform2DComponent& transform) {
		transform.position += rigid.velocity * dt * 16.f; // scale by 16 as 16 is ~ 1m in game unit
	});
}

}