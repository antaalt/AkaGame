#include "MenuWidget.h"

namespace aka {

MenuWidget::MenuWidget() :
	m_vsync(true),
	m_fullscreen(false)
{
}

void MenuWidget::update(World &world)
{
	// Screenshot
	if (input::down(input::Key::F1))
	{
		GraphicBackend::screenshot("./output.jpg");
		Logger::info("Screenshot taken.");
	}
	if (input::down(input::Key::F2))
	{
		m_vsync = !m_vsync;
		GraphicBackend::vsync(m_vsync);
		Logger::info("Vsync ", m_vsync ? "enabled" : "disabled");
	}
	if (input::down(input::Key::F3))
	{
		m_fullscreen = !m_fullscreen;
		PlatformBackend::setFullscreen(m_fullscreen);
		Logger::info("Fullscreen ", m_fullscreen ? "enabled" : "disabled");
	}
}

void MenuWidget::draw(World& world)
{
	if (ImGui::BeginMainMenuBar())
	{
        if (ImGui::BeginMenu("Edit"))
        {
            if (ImGui::MenuItem("Load"))
            {
				// TODO load a scene
            }
            if (ImGui::MenuItem("Quit"))
            {
				// TODO emit quit event
            }
			if (ImGui::MenuItem("Screenshot", "F1"))
			{
				GraphicBackend::screenshot("./output.jpg");
				Logger::info("Screenshot taken.");
			}
            ImGui::Separator();
            if (ImGui::BeginMenu("Config"))
            {
                if (ImGui::MenuItem("Vsync", "F2", &m_vsync))
                {
                    GraphicBackend::vsync(m_vsync);
					Logger::info("Vsync ", m_vsync ? "enabled" : "disabled");
                }
				if (ImGui::MenuItem("Fullscreen", "F3", &m_fullscreen))
				{
					PlatformBackend::setFullscreen(m_fullscreen);
					Logger::info("Fullscreen ", m_fullscreen ? "enabled" : "disabled");
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