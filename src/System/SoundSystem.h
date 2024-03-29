#pragma once

#include <Aka/OS/FileSystem.h>
#include <Aka/Scene/System.h>
#include <Aka/Scene/Entity.h>
#include <Aka/Audio/AudioStream.h>

namespace aka {

class SoundSystem : public System
{
public:
	void onCreate(World& world) override;
	void onDestroy(World& world) override;
	void onUpdate(World& world, Time::Unit deltaTime) override;
};

};
