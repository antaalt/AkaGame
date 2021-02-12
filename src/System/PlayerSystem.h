#pragma once

#include <Aka/Scene/System.h>
#include "CollisionSystem.h"

namespace aka {

class PlayerSystem : public System
{
public:
	void create(World& world) override;
	void destroy(World& world) override;
	void receive(const CollisionEvent& event);
	void update(World& world, Time::Unit deltaTime) override;
};

};