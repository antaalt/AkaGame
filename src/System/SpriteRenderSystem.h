#pragma once

#include <Aka/Scene/System.h>
#include <Aka/Graphic/Shader.h>

namespace aka {

class SpriteRenderSystem : public System
{
public:
	void draw(World& world) override;
};

}
