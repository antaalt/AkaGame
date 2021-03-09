#pragma once
#include "EditorUI.h"

#include <Aka/Aka.h>

namespace aka {

class EntityWidget : public EditorUI::Widget
{
public:
	void draw(World& world) override;
private:
	Entity m_currentEntity;
};

};