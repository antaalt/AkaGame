#include "ViewWidget.h"

#include "../Game/Views/IntroView.h"
#include "../Game/Views/MenuView.h"
#include "../Game/Views/GameView.h"
#include "../Game/Views/EndView.h"

namespace aka {

void ViewWidget::draw(World& world)
{
	if (ImGui::Begin("View"))
	{
		if (ImGui::BeginChild("Views", ImVec2(0,0), true))
		{
			selectable<IntroView>("Intro");
			selectable<MenuView>("Menu");
			selectable<GameView>("Game");
			selectable<EndView>("End");
		}
		ImGui::EndChild();
	}
	ImGui::End();
}

void ViewWidget::onReceive(const ViewChangedEvent& event)
{
	// TODO get view type in a way
	m_currentView = event.view;
}

}