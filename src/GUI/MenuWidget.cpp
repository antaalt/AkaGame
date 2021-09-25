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
	if (Keyboard::down(KeyboardKey::F1))
	{
		GraphicDevice* device = GraphicBackend::device();
		Backbuffer::Ptr backbuffer = device->backbuffer();
		Image image(backbuffer->width(), backbuffer->height(), 4, ImageFormat::UnsignedByte);
		device->backbuffer()->download(image.data());
		image.encodePNG("screen.png");
		Logger::info("Screenshot taken.");
	}
	if (Keyboard::down(KeyboardKey::F2))
	{
		m_vsync = !m_vsync;
		Synchronisation sync = Synchronisation::Unlimited;
		if (m_vsync)
			sync = Synchronisation::Vertical;
		GraphicBackend::device()->backbuffer()->set(sync);
		Logger::info("Vsync ", m_vsync ? "enabled" : "disabled");
	}
	if (Keyboard::down(KeyboardKey::F3))
	{
		m_fullscreen = !m_fullscreen;
		PlatformBackend::setFullscreen(m_fullscreen);
		Logger::info("Fullscreen ", m_fullscreen ? "enabled" : "disabled");
	}
	// Pause the GameView
	if (Keyboard::down(KeyboardKey::P))
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
				GraphicDevice* device = GraphicBackend::device();
				Backbuffer::Ptr backbuffer = device->backbuffer();
				Image image(backbuffer->width(), backbuffer->height(), 4, ImageFormat::UnsignedByte);
				device->backbuffer()->download(image.data());
				image.encodePNG("screen.png");
				Logger::info("Screenshot taken.");
			}
            ImGui::Separator();
            if (ImGui::BeginMenu("Config"))
            {
                if (ImGui::MenuItem("Vsync", "F2", &m_vsync))
                {
					Synchronisation sync = Synchronisation::Unlimited;
					if (m_vsync)
						sync = Synchronisation::Vertical;
					GraphicBackend::device()->backbuffer()->set(sync);
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