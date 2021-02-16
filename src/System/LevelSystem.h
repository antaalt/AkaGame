#pragma once

#include <Aka/OS/Time.h>
#include <Aka/Scene/System.h>

#include "../Game/Level.h"

namespace aka {

class LevelSystem : public System
{
public:
	LevelSystem(WorldMap& map);
	void update(World& world, Time::Unit deltaTime) override;
private:
	bool m_transition;
	WorldMap& m_map;
};

};
