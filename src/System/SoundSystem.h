#pragma once

#include <Aka/OS/FileSystem.h>
#include <Aka/Scene/System.h>
#include <Aka/Scene/Entity.h>
#include <Aka/Audio/AudioStream.h>

namespace aka {

class SoundSystem : public System
{
public:
	void create(World& world) override;
	void destroy(World& world) override;
	void update(World& world, Time::Unit deltaTime) override;
};

};
