#include "Game.h"

#include "Views/GameView.h"
#include "Views/MenuView.h"
#include "Views/EndView.h"
#include "../GUI/EntityWidget.h"
#include "../GUI/InfoWidget.h"
#include "../GUI/ResourcesWidget.h"
#include "../GUI/MenuWidget.h"

namespace aka {

const ViewID Views::menu = generate();
const ViewID Views::game = generate();
const ViewID Views::end = generate();

void Game::initialize()
{
	{
		// Initialize Game view
		router.attach<MenuView>(Views::menu);
		router.attach<GameView>(Views::game, m_world);
		router.attach<EndView>(Views::end);
		router.set(Views::menu);
		current = &router.get();
		current->onCreate();
	}
	{
		// Load global resources
		FontManager::create("Espera48", Font(Asset::path("font/Espera/Espera-Bold.ttf"), 48));
		FontManager::create("Espera16", Font(Asset::path("font/Espera/Espera-Bold.ttf"), 16));
		FontManager::create("BoldFont48", Font(Asset::path("font/Theboldfont/theboldfont.ttf"), 48));
	}
	{
		// Initialize GUI
		m_gui.add<InfoWidget>();
		m_gui.add<EntityWidget>();
		m_gui.add<ResourcesWidget>();
		m_gui.add<MenuWidget>();
		m_gui.initialize();
	}
}

void Game::destroy()
{
	current->onDestroy();
	m_gui.destroy();
}

void Game::start()
{
	View* newView = &router.get();
	if (newView != current)
	{
		current->onDestroy();
		newView->onCreate();
		current = newView;
	}
}

void Game::update(Time::Unit deltaTime)
{
	current->onUpdate(router, deltaTime);
	if (input::pressed(input::Key::Escape))
		quit();
	// Hide the GUI
	if (!m_gui.focused() && input::down(input::Key::H))
	{
		m_gui.setVisible(!m_gui.isVisible());
	}
	// Update interface
	m_gui.update(m_world);
}

void Game::frame()
{
	m_gui.frame();
	current->onFrame();
}

void Game::render()
{
	current->onRender();
	{
		// Rendering imgui
		m_gui.draw(m_world);
		m_gui.render();
	}
}

void Game::present()
{
	current->onPresent();
}

void Game::end()
{

}

};