#pragma once

#include <Aka/Core/Geometry.h>
#include <Aka/Scene/Component.h>
#include <Aka/Graphic/Texture.h>

namespace aka {

struct TileMap
{
	TileMap();
	TileMap(const vec2u& gridCount, const vec2u& gridSize, Texture::Ptr texture);

	vec2u gridCount;
	vec2u gridSize;
	Texture::Ptr texture;
};

}
