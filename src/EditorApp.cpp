#include "EditorApp.h"
#include "EditorApp.h"

#include "Game/Views/GameView.h"
#include "GUI/EntityWidget.h"
#include "GUI/InfoWidget.h"
#include "GUI/ResourcesWidget.h"
#include "GUI/MenuWidget.h"
#include "GUI/ViewWidget.h"

#include "IconsFontAwesome5.h"

#include <Aka/Layer/ImGuiLayer.h>

namespace aka {

World* getWorld(View::Ptr view)
{
	if (typeid(*view.get()) == typeid(GameView))
	{
		GameView* v = reinterpret_cast<GameView*>(view.get());
		Game& g = v->game(); // TODO merge gameview & game.
		return &g.world;
	}
	else
	{
		static World dummyWorld;
		return &dummyWorld;
	}
}

void EditorApp::onCreate()
{
	{
		Device device = Device::getDefault();
		Logger::info("Device vendor : ", device.vendor);
		Logger::info("Device renderer : ", device.renderer);
		Logger::info("Device memory : ", device.memory);
		Logger::info("Device version : ", device.version);
	}

	{
		attach<ImGuiLayer>();
	}

	{
		// INIT fonts
		// TODO application holds assets (provide functions such as load texture, font...)
		FontManager::create("Espera48", Font(Asset::path("font/Espera/Espera-Bold.ttf"), 48));
		FontManager::create("Espera16", Font(Asset::path("font/Espera/Espera-Bold.ttf"), 16));
		FontManager::create("BoldFont48", Font(Asset::path("font/Theboldfont/theboldfont.ttf"), 48));
	}

	{
		m_widgets.push_back(new InfoWidget);
		m_widgets.push_back(new EntityWidget);
		m_widgets.push_back(new ResourcesWidget);
		m_widgets.push_back(new MenuWidget);
		m_widgets.push_back(new ViewWidget);

		// --- Font
		ImGuiIO& io = ImGui::GetIO();
		ImFontConfig config;
		config.MergeMode = true;
		config.GlyphMinAdvanceX = 13.0f; // Use if you want to make the icon monospaced
		static const ImWchar icon_ranges[] = { ICON_MIN_FA, ICON_MAX_FA, 0 };
		Path asset = Asset::path("font/FontAwesome5.15.2/Font Awesome 5 Free-Regular-400.otf");
		Path asset2 = Asset::path("font/FontAwesome5.15.2/Font Awesome 5 Free-Solid-900.otf");
		Path assetDefault = Asset::path("font/OpenSans/OpenSans-Regular.ttf");
		io.FontDefault = io.Fonts->AddFontFromFileTTF(assetDefault.cstr(), 18.0f);
		ImFont* iconFont = io.Fonts->AddFontFromFileTTF(asset.cstr(), 13.0f, &config, icon_ranges);
		ImFont* iconFont2 = io.Fonts->AddFontFromFileTTF(asset2.cstr(), 13.0f, &config, icon_ranges);
		AKA_ASSERT(iconFont != nullptr, "Icon font not loaded");
		AKA_ASSERT(iconFont2 != nullptr, "Icon font not loaded");

		for (EditorWidget* widget : m_widgets)
			widget->initialize();
	}

	{
		m_view = View::create<GameView>();
		m_view->onCreate();
		m_world = getWorld(m_view);
	}
}

void EditorApp::onDestroy()
{
	m_view->onDestroy();

	{
		for (EditorWidget* widget : m_widgets)
		{
			widget->destroy();
			delete widget;
		}
		m_widgets.clear();
	}

	{
		FontManager::destroy("Espera48");
		FontManager::destroy("Espera16");
		FontManager::destroy("BoldFont48");
	}
}

void EditorApp::onFrame()
{
	m_view->onFrame();
}

void EditorApp::onUpdate(Time::Unit deltaTime)
{
	EventDispatcher<PauseGameEvent>::dispatch();
	// Update the game logic
	m_view->onUpdate(deltaTime);

	// Reset
	if (Keyboard::down(KeyboardKey::R))
	{
		// TODO send back to level 1 ?
		EventDispatcher<PlayerDeathEvent>::emit();
	}
	// Quit
	if (Keyboard::pressed(KeyboardKey::Escape))
		EventDispatcher<QuitEvent>::emit();

	// UI
	for (EditorWidget* widget : m_widgets)
		widget->update(*m_world);
}

void EditorApp::onRender()
{
	m_view->onRender();
	for (EditorWidget* widget : m_widgets)
		widget->draw(*m_world);
}

void EditorApp::onResize(uint32_t width, uint32_t height)
{
	m_view->onResize(width, height);
}

void EditorApp::onPresent()
{
	EventDispatcher<ViewChangedEvent>::dispatch();
}

void EditorApp::onReceive(const ViewChangedEvent& event)
{
	if (m_view != event.view)
	{
		m_view->onDestroy();
		m_view = event.view;
		m_view->onCreate();
		m_world = getWorld(m_view);
	}
	else
	{
		Logger::warn("Trying to change view, but same view given.");
	}
}

void EditorApp::onReceive(const PauseGameEvent& event)
{
	if (event.pause != m_paused)
	{
		m_paused = !m_paused;
		if (m_paused)
			AudioBackend::stop();
		else
			AudioBackend::start();
	}
}

};