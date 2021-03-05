#pragma once

#include <Aka/OS/Time.h>
#include <Aka/Core/Geometry.h>

namespace aka {

struct Particle2D {
	// Shape
	// velocity
	Time::Unit birthTime = Time::zero();
	Time::Unit lifeTime = Time::zero();
	vec2f velocity = vec2f(0.f);
	radianf angularVelocity = radianf(0.f);
	vec2f scaleVelocity = vec2f(1.f);
	color4f color = color4f(1.f);
	int32_t layer = 0;
};

};