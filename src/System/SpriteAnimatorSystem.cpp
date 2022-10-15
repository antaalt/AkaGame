#include "SpriteAnimatorSystem.h"

#include <Aka/Scene/World.h>

namespace aka {

void SpriteAnimatorSystem::onUpdate(World& world, Time deltaTime)
{
    auto view = world.registry().view<SpriteAnimatorComponent>();
    for(entt::entity entity : view)
    {
		SpriteAnimatorComponent& animator = world.registry().get<SpriteAnimatorComponent>(entity);
        Time zero = Time();
        if (animator.currentAnimationDuration > zero && animator.sprite != nullptr)
        {
            //animator.animationTimer = (animator.animationTimer + deltaTime) % animator.currentAnimationDuration;
            animator.animationTimer = (animator.animationTimer + deltaTime);
            if (animator.animationTimer >= animator.currentAnimationDuration)
            {
                world.emit<SpriteAnimationFinishedEvent>(SpriteAnimationFinishedEvent(Entity(entity, &world)));
                animator.animationTimer = animator.animationTimer % animator.currentAnimationDuration;
            }
            uint32_t frameID = 0;
            Time currentFrameDuration = zero;
			const SpriteAnimation& animation = animator.sprite->getAnimation(animator.currentAnimation);
            for (const SpriteFrame& frame : animation.frames)
            {
                if (animator.animationTimer < currentFrameDuration + frame.duration)
                    break;
                currentFrameDuration += frame.duration;
                frameID++;
            }
            animator.currentFrame = frameID;
        }
    }
}

}
