#pragma once

#include <Aka/Core/Geometry.h>
#include <Aka/Scene/Component.h>

#include <vector>

namespace aka {

struct TileLayerComponent
{
	TileLayerComponent() : TileLayerComponent(vec2f(0.f), vec2u(0), vec2u(0), color4f(1.f), std::vector<int>(), 0) {}
	TileLayerComponent(const vec2f& offset, const vec2u &gridCount, const vec2u &gridSize, const color4f &color, const std::vector<int> &tileID, int32_t layer) :
		offset(offset), gridCount(gridCount), gridSize(gridSize), color(color), tileID(tileID), layer(layer) {}

	vec2f offset;
	vec2u gridCount;
	vec2u gridSize;
	color4f color;
	std::vector<int> tileID;
	int32_t layer;
};

};

