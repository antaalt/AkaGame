#pragma once

#include <Aka/Core/ECS/System.h>
#include <Aka/Core/ECS/Event.h>
#include "CollisionSystem.h"

namespace aka {

class PlayerSystem : public System
{
public:
	PlayerSystem(World* world);

	void update(Time::Unit deltaTime) override;
};

};