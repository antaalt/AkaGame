#include "SpriteAnimatorSystem.h"

#include <Aka/Scene/World.h>

namespace aka {

void SpriteAnimatorSystem::onUpdate(World& world, Time::Unit deltaTime)
{
    auto view = world.registry().view<SpriteAnimatorComponent>();
    for(entt::entity entity : view)
    {
		SpriteAnimatorComponent& animator = world.registry().get<SpriteAnimatorComponent>(entity);
        Time::Unit zero = Time::Unit();
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
            Time::Unit currentFrameDuration = zero;
            for (Sprite::Frame& frame : animator.sprite->animations[animator.currentAnimation].frames)
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
