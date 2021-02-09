#include "CoinSystem.h"

#include <Aka/Core/ECS/World.h>
#include <Aka/OS/Logger.h>
#include "../Component/Coin.h"
#include "../Component/Animator.h"

namespace aka {

CoinSystem::CoinSystem(World* world) :
	System(world)
{
}

void CoinSystem::update(Time::Unit deltaTime)
{
	m_world->receive<AnimationFinishedEvent>([](AnimationFinishedEvent* event) {
		Coin* coin = event->entity->get<Coin>();
		if (coin == nullptr || !coin->picked)
			return;
		event->entity->destroy();
	});
}

}