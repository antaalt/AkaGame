#pragma	once
#include "GUINode.h"

#include "../Game/Game.h"
#include <Aka/Core/Router.h>

namespace aka {

class ViewWidget : public GUIWidget
{
public:
	ViewWidget(Router& router);
	//void update() override;
	void draw(World& world) override;
private:
	Router& m_router;
};


};