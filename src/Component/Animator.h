#pragma once

#include <Aka/Core/Sprite.h>
#include <Aka/Scene/Component.h>

namespace aka {

class AnimatorSystem;

struct Animator
{
	friend AnimatorSystem;

	Animator();
	Animator(Sprite* sprite, int32_t layer);

	Sprite* sprite;
	uint32_t currentAnimation;
	uint32_t currentFrame;
	int32_t layer;
	bool flipU, flipV;

	Sprite::Frame& getCurrentSpriteFrame() const;

	void play(const std::string& animation);
	void update();
private:
	Time::Unit animationTimer;
	Time::Unit currentAnimationDuration;
};

};

