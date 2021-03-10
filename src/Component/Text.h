#pragma once

#include <Aka/Core/Font.h>
#include <Aka/Scene/Component.h>


namespace aka {

struct Text
{
	Text() : Text(vec2f(0.f), nullptr, "", color4f(1.f), 0) {}
	Text(const vec2f &offset, Font* font, const std::string& text, const color4f& color, int32_t layer) :
		offset(offset), font(font), text(text), color(color), layer(layer) {}

	vec2f offset;
	Font *font;
	std::string text;
	color4f color;
	int32_t layer;
};

}

