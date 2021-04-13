#pragma once
#include "../EditorApp.h"

#include <Aka/Aka.h>

namespace aka {

class EntityWidget : public EditorWidget
{
public:
	void draw(World& world) override;
private:
	Entity m_currentEntity;
};

};