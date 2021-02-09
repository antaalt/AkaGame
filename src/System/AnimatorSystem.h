#pragma once

#include <Aka/Core/ECS/System.h>
#include <Aka/Core/ECS/Event.h>

#include "../Component/Animator.h"

namespace aka {

struct AnimationFinishedEvent : public Event {
	AnimationFinishedEvent(Entity *e) : entity(e) {}

	Entity* entity;
};

class AnimatorSystem: public System
{
public:
	AnimatorSystem(World* world);
public:
	void update(Time::Unit deltaTime) override;
};


}

