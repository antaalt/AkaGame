#pragma once

#include <Aka/Core/ECS/System.h>
#include <Aka/Graphic/Shader.h>

namespace aka {

class TileMapSystem : public System
{
public:
	TileMapSystem(World* world);

	void draw(Batch &batch) override;
};

}