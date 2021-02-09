#pragma once

#include <Aka/Core/ECS/System.h>

namespace aka {

class CameraSystem : public System
{
public:
	CameraSystem(World* world);

	void update(Time::Unit deltaTime) override;
};

};
