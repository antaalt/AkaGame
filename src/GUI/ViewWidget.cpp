#include "ViewWidget.h"

namespace aka {

ViewWidget::ViewWidget(Router& router) :
	m_router(router)
{
}

void ViewWidget::draw(World& world)
{
	if (ImGui::Begin("View"))
	{
		std::map<ViewID, std::string> views;
		views.insert(std::make_pair(Views::intro, "Intro"));
		views.insert(std::make_pair(Views::menu, "Menu"));
		views.insert(std::make_pair(Views::game, "Game"));
		views.insert(std::make_pair(Views::end, "End"));
		if (ImGui::BeginChild("Views", ImVec2(0,0), true))
		{
			for (auto it : views)
				if (ImGui::Selectable(it.second.c_str(), it.first == m_router.current()))
					m_router.set(it.first);
		}
		ImGui::EndChild();
	}
	ImGui::End();
}

}