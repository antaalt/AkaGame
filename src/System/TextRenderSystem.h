#pragma once

#include <Aka/Scene/System.h>
#include <Aka/Graphic/GraphicBackend.h>

namespace aka {

class TextRenderSystem : public System
{
public:
	void draw(World& world) override;
};

}

