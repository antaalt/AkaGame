#pragma once
#include <Aka/Scene/System.h>
#include "../Component/Animator.h"
#include "../System/AnimatorSystem.h"

namespace aka {

class CoinSystem : public System
{
public:
	void create(World& world) override;
	void destroy(World& world) override;
	void receive(const AnimationFinishedEvent& event);
};


};

