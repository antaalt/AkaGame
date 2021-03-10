#include "SoundSystem.h"

#include <Aka/OS/Logger.h>
#include <Aka/Scene/World.h>
#include <Aka/Audio/AudioBackend.h>
#include "../Component/SoundInstance.h"

namespace aka {

void onAudioAdd(entt::registry& registry, entt::entity entity)
{
	SoundInstance& sound = registry.get<SoundInstance>(entity);
	sound.audio->seek(0);
	AudioBackend::play(sound.audio);
}

void onAudioRemove(entt::registry& registry, entt::entity entity)
{
	SoundInstance& sound = registry.get<SoundInstance>(entity);
	AudioBackend::close(sound.audio);
}

void onAudioUpdate(entt::registry& registry, entt::entity entity)
{
	SoundInstance& sound = registry.get<SoundInstance>(entity);
	AudioBackend::play(sound.audio);
}

void SoundSystem::create(World& world)
{
	world.registry().on_construct<SoundInstance>().connect<&onAudioAdd>();
	world.registry().on_destroy<SoundInstance>().connect<&onAudioRemove>();
	world.registry().on_update<SoundInstance>().connect<&onAudioUpdate>();
}

void SoundSystem::destroy(World& world)
{
	world.registry().on_construct<SoundInstance>().disconnect<&onAudioAdd>();
	world.registry().on_destroy<SoundInstance>().disconnect<&onAudioRemove>();
	world.registry().on_update<SoundInstance>().disconnect<&onAudioUpdate>();
}

void SoundSystem::update(World& world, Time::Unit deltaTime)
{
	auto view = world.registry().view<SoundInstance>();
	for (entt::entity entity : view) {
 		SoundInstance& sound = world.registry().get<SoundInstance>(entity);
		if (!sound.audio->playing())
		{
			if (sound.loop)
			{
				sound.audio->seek(0);
				AudioBackend::play(sound.audio);
			}
			else
				world.registry().destroy(entity);
			// TODO do not destroy entity, destroy only component.
			//world.registry().remove<SoundInstance>(entity);
		}
	}
}

};