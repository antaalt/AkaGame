#pragma once

#include <Aka/Scene/System.h>
#include "../Component/Collider2D.h"

namespace aka {

// Which face is the collision
enum class CollisionFace {
	None,
	Top,
	Bottom,
	Right,
	Left,
};

struct CollisionEvent {
	CollisionEvent(Entity s, Entity d, CollisionType type, CollisionFace face);

	Entity staticEntity;
	Entity dynamicEntity;
	CollisionType staticType;
	CollisionFace face;
};

class CollisionSystem : public System
{
public:
	void update(World& world, Time::Unit deltaTime) override;
};

};
