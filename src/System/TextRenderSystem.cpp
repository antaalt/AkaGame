#include "TextRenderSystem.h"

#include <Aka/Scene/World.h>
#include <Aka/Core/Font.h>
#include <Aka/OS/Logger.h>

#include "../Component/Text.h"
#include "../Component/Transform2D.h"

namespace aka {

void TextRenderSystem::onRender(World& world)
{
    auto view = world.registry().view<Text2DComponent, Transform2DComponent>();
    view.each([](Text2DComponent& text, Transform2DComponent& transform) {
		Renderer2D::drawText(
			transform.model() * mat3f::translate(text.offset),
			text.text,
			*text.font, 
			text.color,
			text.layer
		);
    });
}

}