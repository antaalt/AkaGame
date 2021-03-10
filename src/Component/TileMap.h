#pragma once

#include <Aka/Core/Geometry.h>
#include <Aka/Scene/Component.h>
#include <Aka/Graphic/Texture.h>

namespace aka {

struct TileMap
{
	TileMap() : TileMap(vec2u(0), vec2u(0), nullptr) {}
	TileMap(const vec2u& gridCount, const vec2u& gridSize, Texture::Ptr texture) : gridCount(gridCount), gridSize(gridSize), texture(texture) {}

	vec2u gridCount;
	vec2u gridSize;
	Texture::Ptr texture;
};

}
