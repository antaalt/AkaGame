#pragma once

#include <Aka/Core/Font.h>
#include <Aka/Core/ECS/Component.h>


namespace aka {

struct Text : public Component
{
	Text();
	Text(const vec2f &offset, Font* font, const std::string& text, const color4f& color, int32_t layer);

	vec2f offset;
	Font *font;
	std::string text;
	color4f color;
	int32_t layer;
};

}

