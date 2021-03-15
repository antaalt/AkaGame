#include "ParticleSystem.h"

#include <Aka/Scene/World.h>

#include "../Component/Particle.h"
#include "../Component/Transform2D.h"

namespace aka {

void ParticleSystem::update(World& world, Time::Unit deltaTime)
{
	auto view = world.registry().view<Particle2DComponent, Transform2DComponent>();
	// Move particles
	view.each([&](Particle2DComponent& particle, Transform2DComponent& transform) {
		transform.position += particle.velocity * deltaTime.seconds() * 3.f;
		transform.rotation = radianf(transform.rotation() + particle.angularVelocity() * deltaTime.seconds() * 3.f);
		transform.size += particle.scaleVelocity * deltaTime.seconds() * 3.f;
	});
	// Renew particles if finished.
	for (entt::entity entity : view)
	{
		Particle2DComponent& particle = world.registry().get<Particle2DComponent>(entity);
		if (particle.lifeTime == Time::zero())
			return;
		else if (Time::now() - particle.birthTime > particle.lifeTime)
		{
			world.registry().destroy(entity);
			// TODO emit event ?
		}
	}
}

void ParticleSystem::draw(World& world, Batch& batch)
{
	auto view = world.registry().view<Particle2DComponent, Transform2DComponent>();
	view.each([&](Particle2DComponent& particle, Transform2DComponent& transform) {
		mat3f t = mat3f::identity();
		t *= mat3f::translate(transform.position);
		t *= mat3f::translate(vec2f(0.5f * transform.size));
		t *= mat3f::rotate(transform.rotation);
		t *= mat3f::translate(vec2f(-0.5f * transform.size));
		t *= mat3f::scale(transform.size);
		batch.draw(t, Batch::Rect(vec2f(0.f), vec2f(1.f), particle.color, particle.layer));
	});
}

}