#pragma once

#include <Aka/Scene/System.h>

#include "../Component/SpriteAnimator.h"

namespace aka {

struct SpriteAnimationFinishedEvent {
	SpriteAnimationFinishedEvent(Entity e) : entity(e) {}

	Entity entity;
};

class SpriteAnimatorSystem: public System
{
public:
	void onUpdate(World &world, Time deltaTime) override;
};


}

