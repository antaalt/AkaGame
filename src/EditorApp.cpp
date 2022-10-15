#include "EditorApp.h"
#include "EditorApp.h"

#include "Game/Views/GameView.h"
#include "GUI/EntityWidget.h"
#include "GUI/InfoWidget.h"
#include "GUI/ResourcesWidget.h"
#include "GUI/MenuWidget.h"
#include "GUI/ViewWidget.h"

#include "IconsFontAwesome5.h"
#include "Shaders.h"

#include <Aka/Layer/ImGuiLayer.h>

#include <Aka/Resource/AssetImporter.h>
#include <Aka/Resource/Resource/StaticMesh.h>

namespace aka {

World* getWorld(View::Ptr view)
{
	if (typeid(*view.get()) == typeid(GameView))
	{
		GameView* v = reinterpret_cast<GameView*>(view.get());
		return &v->world;
	}
	else
	{
		static World dummyWorld;
		return &dummyWorld;
	}
}

EditorApp::EditorApp() :
	Application(std::vector<Layer*>{ new ImGuiLayer }),
	m_shaderReloadTimeElapsed(Time::zero()),
	m_paused(false)
{
}

void EditorApp::onCreate(int argc, char* argv[])
{
	{
		AssetImporter::import(OS::cwd() + "asset/font/Espera/Espera-Bold.ttf", ResourceType::Font, [&](Asset& asset) {
			asset.load(graphic());
			resource()->add("EsperaBold", asset);
		});
		AssetImporter::import(OS::cwd() + "asset/font/Theboldfont/theboldfont.ttf", ResourceType::Font, [&](Asset& asset) {
			asset.load(graphic());
			resource()->add("TheBoldFont", asset);
		});
		AssetImporter::import(OS::cwd() + "asset/font/OpenSans/OpenSans-Regular.ttf", ResourceType::Font, [&](Asset& asset) {
			asset.load(graphic());
			resource()->add("OpenSans", asset);
		});
	}

	{
		// Load shader descriptions
		ShaderRegistry* registry = program();
		registry->add(ProgramAnimatedSprite, graphic());
		registry->add(ProgramStaticSprite, graphic());
		registry->add(ProgramTilemap, graphic());
	}

	{
		m_widgets.push_back(new InfoWidget);
		m_widgets.push_back(new EntityWidget);
		m_widgets.push_back(new ResourcesWidget);
		m_widgets.push_back(new MenuWidget);
		m_widgets.push_back(new ViewWidget);

		// --- Font
		Asset EsperaBoldAsset = resource()->find("EsperaBold");
		Asset TheBoldFontAsset = resource()->find("TheBoldFont");
		Asset OpenSansAsset = resource()->find("OpenSans");
		Font* EsperaBold = EsperaBoldAsset.get<Font>();
		Font* TheBoldFont = TheBoldFontAsset.get<Font>();
		Font* OpenSans = OpenSansAsset.get<Font>();
		Blob FontAwesomeTTF[2];
		OS::File::read(OS::cwd() + "asset/font/FontAwesome5.15.2/Font Awesome 5 Free-Regular-400.otf", &FontAwesomeTTF[0]);
		OS::File::read(OS::cwd() + "asset/font/FontAwesome5.15.2/Font Awesome 5 Free-Solid-900.otf", &FontAwesomeTTF[1]);
		const Blob& OpenSansTTF = reinterpret_cast<FontBuildData*>(OpenSans->getBuildData())->ttf;

		ImGuiIO& io = ImGui::GetIO();
		ImFontConfig config;
		config.MergeMode = true;
		config.GlyphMinAdvanceX = 13.0f; // Use if you want to make the icon monospaced
		static const ImWchar icon_ranges[] = { ICON_MIN_FA, ICON_MAX_FA, 0 };
		void* ImGuiOpenSansTTF = IM_ALLOC(OpenSansTTF.size()); // ownership transferred
		void* ImGuiFontAwesomeTTF0 = IM_ALLOC(FontAwesomeTTF[0].size()); // ownership transferred
		void* ImGuiFontAwesomeTTF1 = IM_ALLOC(FontAwesomeTTF[1].size()); // ownership transferred
		memcpy(ImGuiOpenSansTTF, OpenSansTTF.data(), OpenSansTTF.size());
		memcpy(ImGuiFontAwesomeTTF0, FontAwesomeTTF[0].data(), FontAwesomeTTF[0].size());
		memcpy(ImGuiFontAwesomeTTF1, FontAwesomeTTF[1].data(), FontAwesomeTTF[1].size());
		io.FontDefault = io.Fonts->AddFontFromMemoryTTF(ImGuiOpenSansTTF, (int)OpenSansTTF.size(), 18.0f);
		ImFont* iconFont = io.Fonts->AddFontFromMemoryTTF(ImGuiFontAwesomeTTF0, (int)FontAwesomeTTF[0].size(), 13.0f, &config, icon_ranges);
		ImFont* iconFont2 = io.Fonts->AddFontFromMemoryTTF(ImGuiFontAwesomeTTF1, (int)FontAwesomeTTF[1].size(), 13.0f, &config, icon_ranges);
		bool buildResult = io.Fonts->Build();
		AKA_ASSERT(buildResult, "Font build failed");
		AKA_ASSERT(io.FontDefault != nullptr, "Icon font not loaded");
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
	AssetRegistry* registry = Application::app()->resource();
	registry->clear();
	m_world->clear();
	m_view->onDestroy();

	{
		for (EditorWidget* widget : m_widgets)
		{
			widget->destroy();
			delete widget;
		}
		m_widgets.clear();
	}
}

void EditorApp::onFrame()
{
	m_view->onFrame();
}

void EditorApp::onFixedUpdate(Time deltaTime)
{
	m_view->onFixedUpdate(deltaTime);
}

void EditorApp::onUpdate(Time deltaTime)
{
	EventDispatcher<PauseGameEvent>::dispatch();
	// Update the game logic
	m_view->onUpdate(deltaTime);
	// Reset
	if (platform()->keyboard().down(KeyboardKey::R))
	{
		// TODO send back to level 1 ?
		EventDispatcher<PlayerDeathEvent>::emit();
	}
	// Quit
	if (platform()->keyboard().pressed(KeyboardKey::Escape))
		EventDispatcher<QuitEvent>::emit();

	// UI
	for (EditorWidget* widget : m_widgets)
		widget->update(*m_world);

	// Shaders
	m_shaderReloadTimeElapsed += deltaTime;
	if (m_shaderReloadTimeElapsed.milliseconds() > 1000)
	{
		m_shaderReloadTimeElapsed = Time::zero();
		program()->reloadIfChanged(graphic());
	}
}

void EditorApp::onRender(gfx::Frame* frame)
{
	m_view->onRender(frame);
	for (EditorWidget* widget : m_widgets)
		widget->draw(*m_world);
}

void EditorApp::onResize(uint32_t width, uint32_t height)
{
	m_view->onResize(width, height);
}

void EditorApp::onPresent()
{
	m_view->onPresent();
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
		//AudioDevice* audio = Application::audio();
		//m_paused = !m_paused;
		//if (m_paused)
		//	audio->stop();
		//else
		//	audio->start();
	}
}

};