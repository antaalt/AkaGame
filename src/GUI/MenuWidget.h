#pragma once 
#include "EditorUI.h"

namespace aka {

class MenuWidget : public EditorUI::Widget
{
public:
	MenuWidget();
	void update(World &world) override;
	void draw(World& world) override;
private:
	// TODO request this from backend
	bool m_vsync;
	bool m_fullscreen;
};

};