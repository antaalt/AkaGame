#pragma once

#include <Aka/OS/Time.h>
#include <Aka/Scene/System.h>
#include "../Component/Collider2D.h"

namespace aka {

// TODO use pixel intersection
// TODO use box2D as lib instead (https://github.com/erincatto/box2d)
// https://2dengine.com/?p=collisions
class PhysicSystem : public System
{
public:

	void onFixedUpdate(World& world, Time::Unit deltaTime) override;
};

};
