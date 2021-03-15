#pragma once

#include <Aka/Scene/System.h>
#include <Aka/Graphic/Shader.h>

namespace aka {

class TileMapRenderSystem : public System
{
public:
	void draw(World& world, Batch &batch) override;
};

}