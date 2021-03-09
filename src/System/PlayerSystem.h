#pragma once

#include <Aka/Scene/System.h>
#include "CollisionSystem.h"

namespace aka {

class PlayerSystem : public System, WorldEventListener<CollisionEvent>
{
public:
	PlayerSystem(World& world);
	void receive(const CollisionEvent& event) override;
	void update(World& world, Time::Unit deltaTime) override;
};

};