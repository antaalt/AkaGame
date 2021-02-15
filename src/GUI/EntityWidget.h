#pragma once
#include "GUINode.h"

#include <Aka/Aka.h>

namespace aka {

class EntityWidget : public GUIWidget
{
public:
	void update(World& world) override;
	void draw(World& world) override;
private:
	Entity m_currentEntity;
};

};