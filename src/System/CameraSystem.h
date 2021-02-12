#pragma once

#include <Aka/Scene/System.h>

namespace aka {

class CameraSystem : public System
{
public:
	void update(World& world, Time::Unit deltaTime) override;
};

};
