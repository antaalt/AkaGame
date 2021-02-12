#pragma once

#include <Aka/OS/FileSystem.h>
#include <Aka/Scene/System.h>
#include <Aka/Scene/Entity.h>
#include <Aka/Audio/AudioBackend.h>

namespace aka {

class SoundSystem;

struct SoundInstance {
	SoundInstance() : SoundInstance("", 1.f, false) {}
	SoundInstance(const Path &path, float volume, bool loop = false) : path(path), volume(volume), loop(loop) {}

	Path path;
	float volume;
	bool loop;

	AudioID audio;
};

class SoundSystem : public System
{
public:
	void create(World& world) override;
	void destroy(World& world) override;
	///void onAudioAdd(const ComponentAddedEvent<SoundInstance>&);
	//void onAudioRemove(const ComponentRemovedEvent<SoundInstance>&);
	void update(World& world, Time::Unit deltaTime) override;
};

};
