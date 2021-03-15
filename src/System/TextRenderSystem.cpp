#include "TextRenderSystem.h"

#include <Aka/Scene/World.h>
#include <Aka/Core/Font.h>
#include <Aka/OS/Logger.h>

#include "../Component/Text.h"
#include "../Component/Transform2D.h"

namespace aka {

void TextRenderSystem::draw(World& world, Batch& batch)
{
    auto view = world.registry().view<Text2DComponent, Transform2DComponent>();
    view.each([&batch](Text2DComponent& text, Transform2DComponent& transform) {
		batch.draw(
			transform.model() * mat3f::translate(text.offset),
			Batch::Text(text.text, text.font, text.color, text.layer)
		);
    });
}

}