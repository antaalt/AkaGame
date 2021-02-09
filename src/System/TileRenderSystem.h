#pragma once

#include <Aka/Core/ECS/System.h>
#include <Aka/Graphic/Shader.h>

namespace aka {

class TileSystem : public System
{
public:
	TileSystem(World* world);

	void draw(Batch &batch) override;
};

}
