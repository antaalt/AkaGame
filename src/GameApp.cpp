#include "GameApp.h"

#include "Game/Views/IntroView.h"

namespace aka {

void GameApp::onCreate(int argc, char* argv[])
{
	FontManager::create("Espera48", Font(ResourceManager::path("font/Espera/Espera-Bold.ttf"), 48));
	FontManager::create("Espera16", Font(ResourceManager::path("font/Espera/Espera-Bold.ttf"), 16));
	FontManager::create("BoldFont48", Font(ResourceManager::path("font/Theboldfont/theboldfont.ttf"), 48));
	
	m_view = View::create<IntroView>();
	m_view->onCreate();
}

void GameApp::onDestroy()
{
	m_view->onDestroy();
	FontManager::destroy("Espera48");
	FontManager::destroy("Espera16");
	FontManager::destroy("BoldFont48");
}

void GameApp::onFrame()
{
	m_view->onFrame();
}

void GameApp::onUpdate(Time::Unit deltaTime)
{
	m_view->onUpdate(deltaTime);
}

void GameApp::onRender()
{
	m_view->onRender();
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