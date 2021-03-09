#pragma once
#include "EditorUI.h"

namespace aka {

class InfoWidget : public EditorUI::Widget
{
public:
	void draw(World& world) override;
};

};