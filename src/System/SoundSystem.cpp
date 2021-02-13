#include "SoundSystem.h"

#include <Aka/OS/Logger.h>
#include <Aka/Scene/World.h>
#include <Aka/Audio/AudioBackend.h>

namespace aka {

void onAudioAdd(entt::registry& registry, entt::entity entity)
{
	SoundInstance& sound = registry.get<SoundInstance>(entity);
	sound.audio->seek(0);
	AudioBackend::play(sound.audio, sound.loop);
}

void onAudioRemove(entt::registry& registry, entt::entity entity)
{
	SoundInstance& sound = registry.get<SoundInstance>(entity);
	sound.audio->seek(0);
	AudioBackend::close(sound.audio);
}

void SoundSystem::create(World& world)
{
	world.registry().on_construct<SoundInstance>().connect<&onAudioAdd>();
	world.registry().on_destroy<SoundInstance>().connect<&onAudioRemove>();
}

void SoundSystem::destroy(World& world)
{
	world.registry().on_construct<SoundInstance>().disconnect<&onAudioAdd>();
	world.registry().on_destroy<SoundInstance>().disconnect<&onAudioRemove>();
}

void SoundSystem::update(World& world, Time::Unit deltaTime)
{
	auto view = world.registry().view<SoundInstance>();
	for (entt::entity entity : view) {
 		SoundInstance& sound = world.registry().get<SoundInstance>(entity);
		if (AudioBackend::exist(sound.audio))
		{
			AudioBackend::setVolume(sound.audio, sound.volume);
			if (!sound.audio->playing())
			{
				// TODO do not destroy entity.
				world.registry().destroy(entity);
				//world.registry().remove<SoundInstance>(entity);
			}
		}
	}
}

};