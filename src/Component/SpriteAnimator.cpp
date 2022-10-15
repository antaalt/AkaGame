#include "SpriteAnimator.h"

#include <Aka/Core/Config.h>
#include "../System/RenderSystem.h"

namespace aka {

SpriteAnimatorComponent::SpriteAnimatorComponent() :
    SpriteAnimatorComponent(nullptr, 0)
{
}

SpriteAnimatorComponent::SpriteAnimatorComponent(Sprite* sprite, int32_t layerDepth) :
    sprite(sprite),
    currentAnimation(0),
    currentFrame(0),
	layerDepth(layerDepth),
    flipU(false),
    flipV(false),
    animationTimer(),
    currentAnimationDuration()
{
	update();
}

const SpriteAnimation& SpriteAnimatorComponent::getCurrentSpriteAnimation() const
{
	return sprite->getAnimation(currentAnimation);
}

const SpriteFrame& SpriteAnimatorComponent::getCurrentSpriteFrame() const
{
    return sprite->getFrame(currentAnimation, currentFrame);
}

void SpriteAnimatorComponent::play(const String& animationName, bool restart)
{
    const SpriteAnimation& animation = sprite->getAnimation(animationName.cstr());
	uint32_t animIndex = sprite->getAnimationIndex(animationName.cstr());
	//AKA_ASSERT(a != nullptr, "No valid animation");
	bool same = animIndex == currentAnimation;
    currentAnimation = animIndex;
	if (restart)
	{
		currentFrame = 0;
		animationTimer = Time();
		currentAnimationDuration = animation.getDuration();
	}
	else if (!same)
	{
		currentFrame = currentFrame % animation.frames.size();
		currentAnimationDuration = animation.getDuration();
	}
}

void SpriteAnimatorComponent::update()
{
	const SpriteAnimation& animation = sprite->getAnimation(currentAnimation);
    animationTimer = Time();
    currentAnimationDuration = animation.getDuration();
}

}