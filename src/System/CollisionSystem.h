#pragma once

#include <Aka/Scene/System.h>
#include "../Component/Collider2D.h"

namespace aka {

struct CollisionEvent {
	CollisionEvent(Entity s, Entity d, CollisionType type);

	Entity staticEntity;
	Entity dynamicEntity;
	CollisionType staticType;
};

class CollisionSystem : public System
{
public:
	void update(World& world, Time::Unit deltaTime) override;
};

};
