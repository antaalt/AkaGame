#pragma once

#include <Aka/Core/ECS/System.h>
#include <Aka/Graphic/GraphicBackend.h>

namespace aka {

class TextRenderSystem : public System
{
public:
	TextRenderSystem(World* world);

	void draw(Batch &batch) override;
};

}

