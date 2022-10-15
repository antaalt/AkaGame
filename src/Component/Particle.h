#pragma once

#include <Aka/OS/Time.h>
#include <Aka/Resource/Resource/StaticMesh.h>
#include <Aka/Core/Geometry.h>

namespace aka {

struct Particle2DComponent {
	// Shape
	// velocity
	Time birthTime = Time::zero();
	Time lifeTime = Time::zero();
	vec2f velocity = vec2f(0.f);
	anglef angularVelocity = anglef::radian(0.f);
	vec2f scaleVelocity = vec2f(1.f);
	color4f color = color4f(1.f);
	int32_t layer = 0;

	StaticMesh* mesh;
};

};