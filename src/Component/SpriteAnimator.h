#pragma once

#include <Aka/Scene/Component.h>
#include <Aka/Resource/Resource/Sprite.h>

namespace aka {

class SpriteAnimatorSystem;

struct SpriteAnimatorComponent
{
	friend class SpriteAnimatorSystem;

	SpriteAnimatorComponent();
	SpriteAnimatorComponent(Sprite* sprite, int32_t layerDepth);

	Sprite* sprite;
	uint32_t currentAnimation;
	uint32_t currentFrame;
	int32_t layerDepth;
	bool flipU, flipV;

	const SpriteAnimation& getCurrentSpriteAnimation() const;
	const SpriteFrame& getCurrentSpriteFrame() const;

	// Play a new animation and restart or not the timer
	void play(const String& animation, bool restart = false);
	// Reset the animation
	void update();
private:
	Time animationTimer;
	Time currentAnimationDuration;
};

};

