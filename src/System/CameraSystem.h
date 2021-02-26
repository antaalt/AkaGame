#pragma once

#include <Aka/Scene/System.h>

#include "../Game/Level.h"

namespace aka {

class CameraSystem : public System
{
public:
	CameraSystem(WorldMap& map);
	void update(World& world, Time::Unit deltaTime) override;
private:
	WorldMap& m_map;
};

};
