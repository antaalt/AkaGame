#include "CollisionSystem.h"

#include <Aka/Core/Physic.h>
#include <Aka/Scene/World.h>

#include "../Component/Transform2D.h"

namespace aka {

CollisionEvent::CollisionEvent(Entity d, Entity s, CollisionType type) :
	staticEntity(s),
	dynamicEntity(d),
	staticType(type)
{
}

void CollisionSystem::update(World& world, Time::Unit deltaTime)
{
	float dt = deltaTime.seconds();

	auto viewDynamic = world.registry().view<RigidBody2D, Transform2D, Collider2D>();
	auto viewStatic = world.registry().view<Transform2D, Collider2D>();
	for (entt::entity entityDynamic : viewDynamic)
	{
		Transform2D& transformDynamic = world.registry().get<Transform2D>(entityDynamic);
		RigidBody2D& rigidBodyDynamic = world.registry().get<RigidBody2D>(entityDynamic);
		Collider2D& colliderDynamic = world.registry().get<Collider2D>(entityDynamic);

		Rect2D rectDynamic;
		rectDynamic.pos = transformDynamic.model.multiplyPoint(colliderDynamic.position);
		rectDynamic.size = transformDynamic.model.multiplyVector(colliderDynamic.size);

		for (entt::entity entityStatic : viewStatic)
		{
			Transform2D& transformStatic = world.registry().get<Transform2D>(entityStatic);
			Collider2D& colliderStatic = world.registry().get<Collider2D>(entityStatic);

			// Skip self intersection
			if (entityDynamic == entityStatic)
				continue;

			Rect2D rectStatic;
			rectStatic.pos = transformStatic.model.multiplyPoint(colliderStatic.position);
			rectStatic.size = transformStatic.model.multiplyVector(colliderStatic.size);
			Collision2D c = overlap(rectDynamic, rectStatic);
			if (c.collided)
			{
				if (colliderStatic.is(CollisionType::Solid))
				{
					// Adjust velocity
					// Get normal 
					vec2f normal = vec2f::normalize(c.separation);
					// Get relative velocity
					vec2f v = rigidBodyDynamic.velocity; // substract by second object velocity if it has one
					// Get penetration speed
					float ps = vec2f::dot(v, normal);
					// objects moving towards each other ?
					if (ps > 0.f)
						continue;
					// Get penetration component
					vec2f p = normal * ps;
					// tangent component
					vec2f t = v - p;
					// Restitution
					float r = 1.f + max<float>(colliderDynamic.bouncing, colliderStatic.bouncing); // max bouncing value of object a & b
					// Friction
					float f = min<float>(colliderDynamic.friction, colliderStatic.friction); // max friction value of object a & b
					// Change the velocity of shape a
					rigidBodyDynamic.acceleration = vec2f(0);
					rigidBodyDynamic.velocity = rigidBodyDynamic.velocity - p * r + t * f;
					if (world.registry().has<RigidBody2D>(entityStatic)) {
						RigidBody2D& otherRigid = world.registry().get<RigidBody2D>(entityStatic);
						otherRigid.acceleration = vec2f(0.f);
						otherRigid.velocity = otherRigid.velocity + (p * r + t * f);
					}
					// Move the rigid & its collider to avoid overlapping.
					transformDynamic.translate(c.separation);
				}
				world.emit<CollisionEvent>(CollisionEvent(Entity(entityDynamic, &world), Entity(entityStatic, &world), colliderStatic.type));
			}
		}
	}
}

};