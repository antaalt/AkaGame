#include "CoinSystem.h"

#include <Aka/Scene/World.h>
#include <Aka/OS/Logger.h>
#include "../Component/Coin.h"
#include "../Component/SpriteAnimator.h"

namespace aka {

CoinSystem::CoinSystem(World& world) :
	WorldEventListener<SpriteAnimationFinishedEvent>(world)
{
}

void CoinSystem::receive(const SpriteAnimationFinishedEvent& event)
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