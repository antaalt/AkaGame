#include "GameView.h"

#include "../../Component/Transform2D.h"
#include "../../Component/Camera2D.h"

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
		FramebufferAttachment attachment;
		attachment.type = FramebufferAttachmentType::Color0;
		attachment.texture = Texture::create(
			width, height,
			TextureFormat::Byte,
			TextureComponent::RGBA,
			TextureFlag::RenderTarget,
			Sampler::nearest()
		);
		m_framebuffer = Framebuffer::create(width, height, &attachment, 1);
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
	EventDispatcher<PauseGameEvent>::dispatch();
	// Update the game logic
	if (!m_paused)
	{
		// Update world after moving manually objects
		m_game.update(deltaTime);
	}
	else
	{
		Transform2DComponent& t = m_game.camera.entity.get<Transform2DComponent>();
		t.position.x += (Keyboard::pressed(KeyboardKey::ArrowRight) - Keyboard::pressed(KeyboardKey::ArrowLeft)) * deltaTime.seconds() * 50;
		t.position.y += (Keyboard::pressed(KeyboardKey::ArrowUp) - Keyboard::pressed(KeyboardKey::ArrowDown)) * deltaTime.seconds() * 50;
	}

	// Reset
	if (Keyboard::down(KeyboardKey::R))
	{
		// TODO send back to level 1
		EventDispatcher<PlayerDeathEvent>::emit();
	}
	if (Keyboard::pressed(KeyboardKey::Escape))
		EventDispatcher<QuitEvent>::emit(QuitEvent());
	// Update interface
	m_gui.update(m_game.world);
}

void GameView::onRender()
{
	{
		// Render to framebuffer
		Transform2DComponent& cameraTransform = m_game.camera.entity.get<Transform2DComponent>();
		Camera2DComponent& camera = m_game.camera.entity.get<Camera2DComponent>();
		mat4f view = mat4f::inverse(mat4f::from2D(cameraTransform.model()));
		mat4f projection = camera.camera.perspective();
		m_framebuffer->clear(color4f(1.f, 0.63f, 0.f, 1.f)); 
		m_game.draw();
		Renderer2D::render(m_framebuffer, view, projection);
		Renderer2D::clear();
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
	m_game.camera.entity.get<Camera2DComponent>().camera.viewport = vec2f(newWidth, newHeight);
}

void GameView::onReceive(const PauseGameEvent& event)
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

}