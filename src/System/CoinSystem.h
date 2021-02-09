#pragma once
#include <Aka/Core/ECS/System.h>
#include <Aka/Core/ECS/Event.h>
#include "../Component/Animator.h"
#include "../System/AnimatorSystem.h"

namespace aka {

class CoinSystem : public System
{
public:
	CoinSystem(World* world);
public:
	void update(Time::Unit deltaTime) override;
};


};

