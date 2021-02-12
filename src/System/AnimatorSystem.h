#pragma once

#include <Aka/Scene/System.h>

#include "../Component/Animator.h"

namespace aka {

struct AnimationFinishedEvent {
	AnimationFinishedEvent(Entity e) : entity(e) {}

	Entity entity;
};

class AnimatorSystem: public System
{
public:
	void update(World &world, Time::Unit deltaTime) override;
};


}

