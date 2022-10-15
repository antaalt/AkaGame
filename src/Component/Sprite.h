#pragma once

#include <Aka/Scene/Component.h>
#include <Aka/Resource/Resource/Texture.h>

namespace aka {

struct SpriteComponent
{
	SpriteComponent() : SpriteComponent(nullptr, 0) {}
	SpriteComponent(Texture* sprite, int32_t layerDepth) : sprite(sprite), layerDepth(layerDepth) {}

	Texture* sprite;
	int32_t layerDepth;
};

};

