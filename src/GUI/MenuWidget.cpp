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
	PlatformDevice* platform = Application::app()->platform();
	gfx::GraphicDevice* graphic = Application::app()->graphic();
	/*if (platform->keyboard().down(KeyboardKey::F1))
	{
		Backbuffer::Ptr backbuffer = graphic->backbuffer();
		Image image(backbuffer->width(), backbuffer->height(), 4, ImageFormat::UnsignedByte);
		graphic->backbuffer()->download(image.data());
		image.encodePNG("screen.png");
		Logger::info("Screenshot taken.");
	}
	if (platform->keyboard().down(KeyboardKey::F2))
	{
		m_vsync = !m_vsync;
		Synchronisation sync = Synchronisation::Unlimited;
		if (m_vsync)
			sync = Synchronisation::Vertical;
		graphic->backbuffer()->set(sync);
		Logger::info("Vsync ", m_vsync ? "enabled" : "disabled");
	}*/
	if (platform->keyboard().down(KeyboardKey::F3))
	{
		m_fullscreen = !m_fullscreen;
		platform->fullscreen(m_fullscreen);
		Logger::info("Fullscreen ", m_fullscreen ? "enabled" : "disabled");
	}
	// Pause the GameView
	if (platform->keyboard().down(KeyboardKey::P))
	{
		EventDispatcher<PauseGameEvent>::emit(PauseGameEvent{ !m_pause });
	}
}

void MenuWidget::draw(World& world)
{
	PlatformDevice* platform = Application::app()->platform();
	gfx::GraphicDevice* graphic = Application::app()->graphic();
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
				/*Backbuffer::Ptr backbuffer = graphic->backbuffer();
				Image image(backbuffer->width(), backbuffer->height(), 4, ImageFormat::UnsignedByte);
				graphic->backbuffer()->download(image.data());
				image.encodePNG("screen.png");
				Logger::info("Screenshot taken.");*/
			}
            ImGui::Separator();
            if (ImGui::BeginMenu("Config"))
            {
				/*if (ImGui::MenuItem("Vsync", "F2", &m_vsync))
                {
					Synchronisation sync = Synchronisation::Unlimited;
					if (m_vsync)
						sync = Synchronisation::Vertical;
					graphic->backbuffer()->set(sync);
					Logger::info("Vsync ", m_vsync ? "enabled" : "disabled");
                }*/
				if (ImGui::MenuItem("Fullscreen", "F3", &m_fullscreen))
				{
					platform->fullscreen(m_fullscreen);
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