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
		GraphicBackend::screenshot("./output.png");
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
	// Pause the GameView
	if (input::down(input::Key::P))
	{
		EventDispatcher<PauseGameEvent>::emit(PauseGameEvent{ !m_pause });
	}
}

void MenuWidget::draw(World& world)
{
	if (ImGui::BeginMainMenuBar())
	{
		// Edit
        if (ImGui::BeginMenu("Edit"))
        {
            if (ImGui::MenuItem("Load"))
            {
				// TODO load a scene
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
			ImGui::Separator();
			if (ImGui::MenuItem("Quit"))
			{
				EventDispatcher<QuitEvent>::emit();
				Logger::info("Quitting app.");
			}
            ImGui::EndMenu();
        }
		// Play
        if (ImGui::MenuItem(m_pause ? "Play" : "Pause", "P"))
        {
			EventDispatcher<PauseGameEvent>::emit(PauseGameEvent{ !m_pause });
        }
		ImGui::EndMainMenuBar();
	}
}

void MenuWidget::onReceive(const PauseGameEvent& event)
{
	m_pause = event.pause;
}

};