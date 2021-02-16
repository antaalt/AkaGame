#include "TileRenderSystem.h"

#include <Aka/OS/FileSystem.h>
#include <Aka/Scene/World.h>
#include "../Component/Camera2D.h"
#include "../Component/Transform2D.h"
#include "../Component/Animator.h"

namespace aka {

void TileSystem::draw(World& world, Batch &batch)
{
    auto view = world.registry().view<Animator, Transform2D>();
    view.each([&batch](Animator& animator, Transform2D& transform)
    {
        uv2f uv0 = uv2f(0.f);
        uv2f uv1 = uv2f(1.f);
        if (animator.flipU)
        {
            uv0.u = 1.f - uv0.u;
            uv1.u = 1.f - uv1.u;
        }
        if (animator.flipV)
        {
            uv0.v = 1.f - uv0.v;
            uv1.v = 1.f - uv1.v;
        }
        const Sprite::Frame &currentFrame = animator.getCurrentSpriteFrame();
        const Texture::Ptr texture = currentFrame.texture;
        vec2f position = vec2f(0.f);
        vec2f size = vec2f((float)currentFrame.width, (float)currentFrame.height);
        batch.draw(transform.model(), Batch::Rect(position, size, uv0, uv1, texture, animator.layer));
    });
}

}
