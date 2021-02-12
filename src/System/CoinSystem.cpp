#include "CoinSystem.h"

#include <Aka/Scene/World.h>
#include <Aka/OS/Logger.h>
#include "../Component/Coin.h"
#include "../Component/Animator.h"

namespace aka {

void CoinSystem::create(World& world)
{
	world.dispatcher().sink<AnimationFinishedEvent>().connect<&CoinSystem::receive>(*this);
}

void CoinSystem::destroy(World& world)
{
	world.dispatcher().sink<AnimationFinishedEvent>().disconnect<&CoinSystem::receive>(*this);
}

void CoinSystem::receive(const AnimationFinishedEvent& event)
{
	Entity entity = event.entity;
	if (entity.has<Coin>())
	{
		const Coin& coin = entity.get<Coin>();
		if (coin.picked)
		{
			entity.destroy();
		}
	}
}

}