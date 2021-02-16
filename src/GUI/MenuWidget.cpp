#include "MenuWidget.h"

namespace aka {

void MenuWidget::draw(World& world)
{
	if (ImGui::BeginMainMenuBar())
	{
        if (ImGui::BeginMenu("Edit"))
        {
            if (ImGui::MenuItem("Load"))
            {
            }
            if (ImGui::MenuItem("Quit"))
            {
            }
            ImGui::Separator();
            if (ImGui::BeginMenu("Config"))
            {
                static bool vsync = true;
                if (ImGui::MenuItem("Vsync", "", &vsync))
                {
                    GraphicBackend::vsync(vsync);
                }
                ImGui::EndMenu();
            }
            ImGui::EndMenu();
        }
        if (ImGui::MenuItem("Play"))
        {
            // pause update
        }
        ImGui::Separator();
        if (ImGui::MenuItem("Another"))
        {
        }
		ImGui::EndMainMenuBar();
	}
}

};