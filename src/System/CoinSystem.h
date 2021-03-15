#pragma once
#include <Aka/Scene/System.h>
#include "../Component/SpriteAnimator.h"
#include "../System/SpriteAnimatorSystem.h"

namespace aka {

class CoinSystem : public System, WorldEventListener<SpriteAnimationFinishedEvent>
{
public:
	CoinSystem(World& world);
	void receive(const SpriteAnimationFinishedEvent& event) override;
};


};

