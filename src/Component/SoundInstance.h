#pragma once

#include <Aka/Scene/Component.h>
#include <Aka/Audio/AudioStream.h>

namespace aka {

struct SoundInstance {
	SoundInstance() : SoundInstance(nullptr, 1.f, false) {}
	SoundInstance(AudioStream::Ptr audio, float volume, bool loop = false) : audio(audio), loop(loop) { audio->setVolume(volume); }

	AudioStream::Ptr audio;
	bool loop;
};

}

