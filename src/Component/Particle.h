#pragma once

#include <Aka/OS/Time.h>
#include <Aka/Core/Geometry.h>

namespace aka {

struct Particle2D {
	// Shape
	// velocity
	Time::Unit birthTime = Time::zero();
	Time::Unit lifeTime = Time::zero();
	vec2f velocity;
	color4f color;
	int32_t layer;
};

};