#pragma once

#include <Aka/Resource/Resource/Font.h>
#include <Aka/Scene/Component.h>


namespace aka {

struct Text2DComponent
{
	Text2DComponent() : Text2DComponent(vec2f(0.f), nullptr, "", color4f(1.f), 0) {}
	Text2DComponent(const vec2f &offset, Font* font, const String& text, const color4f& color, int32_t layerDepth) :
		offset(offset), font(font), text(text), color(color), layerDepth(layerDepth) {}

	vec2f offset;
	Font *font;
	String text;
	color4f color;
	int32_t layerDepth;
};

}

