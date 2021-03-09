#pragma once
#include <Aka/Scene/System.h>
#include "../Component/Animator.h"
#include "../System/AnimatorSystem.h"

namespace aka {

class CoinSystem : public System, WorldEventListener<AnimationFinishedEvent>
{
public:
	CoinSystem(World& world);
	void receive(const AnimationFinishedEvent& event) override;
};


};

