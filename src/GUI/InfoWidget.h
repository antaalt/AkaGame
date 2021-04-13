#pragma once

#include "../EditorApp.h"

namespace aka {

class InfoWidget : public EditorWidget
{
public:
	void draw(World& world) override;
};

};