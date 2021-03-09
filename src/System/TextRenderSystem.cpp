#include "TextRenderSystem.h"

#include <Aka/Scene/World.h>
#include <Aka/Core/Font.h>
#include <Aka/OS/Logger.h>

#include "../Component/Text.h"
#include "../Component/Transform2D.h"

namespace aka {

void TextRenderSystem::draw(World& world, Batch& batch)
{
    auto view = world.registry().view<Text, Transform2D>();
    view.each([&batch](Text& text, Transform2D& transform) {
		batch.draw(
			transform.model() * mat3f::translate(text.offset),
			Batch::Text(text.text, text.font, text.color, text.layer)
		);
    });
}

}