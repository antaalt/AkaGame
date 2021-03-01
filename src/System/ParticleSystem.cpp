#include "ParticleSystem.h"

#include <Aka/Scene/World.h>

#include "../Component/Particle.h"
#include "../Component/Transform2D.h"

namespace aka {

void ParticleSystem::update(World& world, Time::Unit deltaTime)
{
	auto view = world.registry().view<Particle2D, Transform2D>();
	view.each([&](Particle2D& particle, Transform2D& transform) {
		transform.position += mat3f::rotate(transform.rotation) * particle.velocity * deltaTime.seconds() * 3.f;
	});
}

void ParticleSystem::draw(World& world, Batch& batch)
{
	auto view = world.registry().view<Particle2D, Transform2D>();
	view.each([&](Particle2D& particle, Transform2D& transform) {
		batch.draw(transform.model(), Batch::Rect(vec2f(0.f), vec2f(1.f), particle.color, particle.layer));
	});
}

}