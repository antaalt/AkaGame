#include "SpriteAnimator.h"

#include <Aka/Core/Debug.h>

namespace aka {

SpriteAnimatorComponent::SpriteAnimatorComponent() :
    SpriteAnimatorComponent(nullptr, 0)
{
}

SpriteAnimatorComponent::SpriteAnimatorComponent(Sprite* sprite, int32_t layer) :
    sprite(sprite),
    currentAnimation(0),
    currentFrame(0),
    layer(layer),
    flipU(false),
    flipV(false),
    animationTimer(),
    currentAnimationDuration()
{
}

const Sprite::Animation& SpriteAnimatorComponent::getCurrentSpriteAnimation() const
{
	return sprite->animations[currentAnimation];
}

const Sprite::Frame& SpriteAnimatorComponent::getCurrentSpriteFrame() const
{
    return sprite->getFrame(currentAnimation, currentFrame);
}

void SpriteAnimatorComponent::play(const String& animation, bool restart)
{
    Sprite::Animation* a = sprite->getAnimation(animation);
	AKA_ASSERT(a != nullptr, "No valid animation");
	bool same = (a - sprite->animations.data()) == currentAnimation;
    currentAnimation = static_cast<uint32_t>(a - sprite->animations.data());
	if (restart)
	{
		currentFrame = 0;
		animationTimer = Time::Unit();
		currentAnimationDuration = a->duration();
	}
	else if (!same)
	{
		currentFrame = currentFrame % sprite->animations[currentAnimation].frames.size();
		currentAnimationDuration = a->duration();
	}
}

void SpriteAnimatorComponent::update()
{
    animationTimer = Time::Unit();
    currentAnimationDuration = sprite->animations[currentAnimation].duration();
}

}