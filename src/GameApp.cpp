#include "GameApp.h"

#include <Aka/Resource/AssetImporter.h>

#include "Game/Views/IntroView.h"

namespace aka {

void GameApp::onCreate(int argc, char* argv[])
{
	AssetImporter::import("font/Espera/Espera-Bold.ttf", ResourceType::Font, [&](Asset& asset) {
		asset.load(graphic());
		resource()->add("EsperaBold", asset);
	});
	AssetImporter::import("font/Theboldfont/theboldfont.ttf", ResourceType::Font, [&](Asset& asset) {
		asset.load(graphic());
		resource()->add("TheBoldFont", asset);
	});
	AssetImporter::import("font/OpenSans/OpenSans-Regular.ttf", ResourceType::Font, [&](Asset& asset) {
		asset.load(graphic());
		resource()->add("OpenSans", asset);
	});
	
	m_view = View::create<IntroView>();
	m_view->onCreate();
}

void GameApp::onDestroy()
{
	m_view->onDestroy();
}

void GameApp::onFrame()
{
	m_view->onFrame();
}

void GameApp::onUpdate(Time deltaTime)
{
	m_view->onUpdate(deltaTime);
}

void GameApp::onRender(gfx::Frame* frame)
{
	m_view->onRender(frame);
}

void GameApp::onResize(uint32_t width, uint32_t height)
{
	m_view->onResize(width, height);
}

void GameApp::onPresent()
{
	EventDispatcher<ViewChangedEvent>::dispatch();
}

void GameApp::onReceive(const ViewChangedEvent& event)
{
	if (m_view != event.view)
	{
		m_view->onDestroy();
		m_view = event.view;
		m_view->onCreate();
	}
	else
	{
		Logger::warn("Trying to change view, but same view given.");
	}
}

};