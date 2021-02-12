#include "TextRenderSystem.h"

#include <Aka/Scene/World.h>
#include <Aka/Core/Font.h>

#include "../Component/Text.h"
#include "../Component/Transform2D.h"

#include <utf8.h>

namespace aka {

void TextRenderSystem::draw(World& world, Batch& batch)
{
    const mat3f model = mat3f::identity();
    auto view = world.registry().view<Text, Transform2D>();
    view.each([model, &batch](Text& text, Transform2D& transform) {
        // iterate through all characters
        float scale = 1.f;
        float advance = transform.model[2].x + text.offset.x;
        std::string::iterator start = text.text.begin();
        std::string::iterator end = text.text.end();
        while (start < end)
        {
            uint32_t c = utf8::next(start, end);
            // TODO check if rendering text out of screen for culling ?
            const Character& ch = text.font->getCharacter(c);
            vec2f position = vec2f(advance + ch.bearing.x, transform.model[2].y + text.offset.y - (ch.size.y - ch.bearing.y)) * scale;
            vec2f size = vec2f((float)ch.size.x, (float)ch.size.y) * scale;
            batch.draw(model, Batch::Rect(position, size, ch.texture.get(0), ch.texture.get(1), ch.texture.texture, text.color, text.layer));
            // now advance cursors for next glyph (note that advance is number of 1/64 pixels)
            advance += ch.advance * scale;
        }
    });
}

}