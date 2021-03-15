#pragma once

#include <Aka/Core/Sprite.h>
#include <Aka/Scene/Component.h>

namespace aka {

class SpriteAnimatorSystem;

struct SpriteAnimatorComponent
{
	friend class SpriteAnimatorSystem;

	SpriteAnimatorComponent();
	SpriteAnimatorComponent(Sprite* sprite, int32_t layer);

	Sprite* sprite;
	uint32_t currentAnimation;
	uint32_t currentFrame;
	int32_t layer;
	bool flipU, flipV;

	const Sprite::Animation& getCurrentSpriteAnimation() const;
	const Sprite::Frame& getCurrentSpriteFrame() const;

	void play(const String& animation);
	void update();
private:
	Time::Unit animationTimer;
	Time::Unit currentAnimationDuration;
};

};

