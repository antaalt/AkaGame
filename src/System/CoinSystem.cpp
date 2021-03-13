#include "CoinSystem.h"

#include <Aka/Scene/World.h>
#include <Aka/OS/Logger.h>
#include "../Component/Coin.h"
#include "../Component/Animator.h"

namespace aka {

CoinSystem::CoinSystem(World& world) :
	WorldEventListener<AnimationFinishedEvent>(world)
{
}

void CoinSystem::receive(const AnimationFinishedEvent& event)
{
	Entity entity = event.entity;
	if (entity.valid() && entity.has<CoinComponent>())
	{
		const CoinComponent& coin = entity.get<CoinComponent>();
		if (coin.picked)
		{
			entity.destroy();
		}
	}
}

}