#pragma once
#include "EditorUI.h"

namespace aka {

class ResourcesWidget : public EditorUI::Widget
{
public:
	void draw(World& world) override;
};

};