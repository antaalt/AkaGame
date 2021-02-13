#pragma once
#include "GUINode.h"

namespace aka {

class InfoWidget : public GUIWidget
{
public:
	//void update() override;
	void draw(World& world) override;
};

};