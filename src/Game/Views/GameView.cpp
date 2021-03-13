#include "GameView.h"

#include "../../Component/Transform2D.h"
#include "../../Component/Collider2D.h"
#include "../../Component/Camera2D.h"
#include "../../Component/Text.h"
#include "../../Component/TileMap.h"
#include "../../Component/TileLayer.h"
#include "../../Component/Player.h"
#include "../../Component/SoundInstance.h"
#include "../../Component/Coin.h"
#include "../../Component/Particle.h"
#include "../../Component/Hurtable.h"
#include "../../System/PhysicSystem.h"
#include "../../System/AnimatorSystem.h"
#include "../../System/TileMapSystem.h"
#include "../../System/TileRenderSystem.h"
#include "../../System/TextRenderSystem.h"
#include "../../System/CollisionSystem.h"
#include "../../System/CoinSystem.h"
#include "../../System/SoundSystem.h"
#include "../../System/ParticleSystem.h"
#include "EndView.h"
#include "../OgmoWorld.h"
#include "../../GUI/EntityWidget.h"
#include "../../GUI/InfoWidget.h"
#include "../../GUI/ResourcesWidget.h"
#include "../../GUI/MenuWidget.h"
#include "../../GUI/ViewWidget.h"


namespace aka {

GameView::GameView() :
	m_paused(false)
{
}
void GameView::onCreate()
{
	Device device = Device::getDefault();
	Logger::info("Device vendor : ", device.vendor);
	Logger::info("Device renderer : ", device.renderer);
	Logger::info("Device memory : ", device.memory);
	Logger::info("Device version : ", device.version);

	{
		// INIT fonts
		FontManager::create("Espera48", Font(Asset::path("font/Espera/Espera-Bold.ttf"), 48));
		FontManager::create("Espera16", Font(Asset::path("font/Espera/Espera-Bold.ttf"), 16));
		FontManager::create("BoldFont48", Font(Asset::path("font/Theboldfont/theboldfont.ttf"), 48));
	}

	uint32_t width = (uint32_t)(GraphicBackend::backbuffer()->width() * Game::resolution.y / (float)GraphicBackend::backbuffer()->height());
	uint32_t height = Game::resolution.y;
	{
		// INIT FRAMEBUFFER
		Sampler sampler;
		sampler.filterMag = aka::Sampler::Filter::Nearest;
		sampler.filterMin = aka::Sampler::Filter::Nearest;
		sampler.wrapS = aka::Sampler::Wrap::Clamp;
		sampler.wrapT = aka::Sampler::Wrap::Clamp;
		m_framebuffer = Framebuffer::create(width, height, sampler);
		PlatformBackend::setLimits(m_framebuffer->width(), m_framebuffer->height(), 0, 0);
	}

	m_game.initialize(width, height);

	{
		m_gui.attach<InfoWidget>();
		m_gui.attach<EntityWidget>();
		m_gui.attach<ResourcesWidget>();
		m_gui.attach<MenuWidget>();
		m_gui.attach<ViewWidget>();
		m_gui.initialize();
	}
}

void GameView::onDestroy()
{
	m_gui.destroy();
	m_game.destroy();
	{
		FontManager::destroy("Espera48");
		FontManager::destroy("Espera16");
		FontManager::destroy("BoldFont48");
	}
}

void GameView::onFrame()
{
	m_gui.frame();
}

void GameView::onUpdate(Time::Unit deltaTime)
{
	// Pause the GameView
	if (input::down(input::Key::P))
	{
		m_paused = !m_paused;
		if (m_paused)
			AudioBackend::stop();
		else
			AudioBackend::start();
	}
	// Update the game logic
	if (!m_paused)
	{
		// Update world after moving manually objects
		m_game.update(deltaTime);
	}

	// Reset
	if (input::down(input::Key::R))
	{
		// TODO send back to level 1
		EventDispatcher<PlayerDeathEvent>::emit();
	}
	if (input::pressed(input::Key::Escape))
		EventDispatcher<QuitEvent>::emit(QuitEvent());
	// Hide the GUI
	if (!m_gui.focused() && input::down(input::Key::H))
	{
		m_gui.setVisible(!m_gui.isVisible());
	}
	// Update interface
	m_gui.update(m_game.world);
}

void GameView::onRender()
{
	{
		// Render to framebuffer
		Transform2D& cameraTransform = m_game.camera.entity.get<Transform2D>();
		Camera2D& camera = m_game.camera.entity.get<Camera2D>();
		mat4f view = mat4f::inverse(mat4f::from2D(cameraTransform.model()));
		mat4f projection = camera.camera.perspective();
		m_framebuffer->clear(1.f, 0.63f, 0.f, 1.f); 
		m_game.draw(m_batch);
		m_batch.render(m_framebuffer, view, projection);
		m_batch.clear();
	}
	
	{
		// Blit to backbuffer
		GraphicBackend::backbuffer()->blit(m_framebuffer, Sampler::Filter::Nearest);
	}
	{
		// Rendering imgui
		m_gui.draw(m_game.world);
		m_gui.render();
	}
}

void GameView::onResize(uint32_t width, uint32_t height)
{
	uint32_t newWidth = (uint32_t)(width * Game::resolution.y / (float)height);
	uint32_t newHeight = Game::resolution.y;

	m_framebuffer->resize(newWidth, newHeight);
	m_game.camera.entity.get<Camera2D>().camera.viewport = vec2f(newWidth, newHeight);
}

}