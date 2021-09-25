#include "GameView.h"

namespace aka {

void GameView::onCreate()
{
	uint32_t width = (uint32_t)(GraphicBackend::device()->backbuffer()->width() * Game::resolution.y / (float)GraphicBackend::device()->backbuffer()->height());
	uint32_t height = Game::resolution.y;
	{
		// INIT FRAMEBUFFER
		Attachment attachment = {
			AttachmentType::Color0,
			Texture2D::create(
				width, height,
				TextureFormat::RGBA8,
				TextureFlag::RenderTarget | TextureFlag::ShaderResource
			)
		};
		m_framebuffer = Framebuffer::create(&attachment, 1);
		PlatformBackend::setLimits(m_framebuffer->width(), m_framebuffer->height(), 0, 0);
	}

	m_game.initialize(width, height);
}

void GameView::onDestroy()
{
	m_game.destroy();
}

void GameView::onFixedUpdate(Time::Unit deltaTime)
{
	m_game.fixedUpdate(deltaTime);
}

void GameView::onUpdate(Time::Unit deltaTime)
{
	m_game.update(deltaTime);
}

void GameView::onRender()
{
	{
		// Render to framebuffer
		m_framebuffer->clear(color4f(1.f, 0.63f, 0.f, 1.f)); 
		m_game.draw();
		Renderer2D::render(m_framebuffer, m_game.camera.view(), m_game.camera.projection());
		Renderer2D::clear();
	}
	
	{
		// Blit to backbuffer
		GraphicBackend::device()->backbuffer()->blit(m_framebuffer->get(AttachmentType::Color0), TextureFilter::Nearest);
	}
}

void GameView::onResize(uint32_t width, uint32_t height)
{
	uint32_t newWidth = (uint32_t)(width * Game::resolution.y / (float)height);
	uint32_t newHeight = Game::resolution.y;

	m_framebuffer->set(AttachmentType::Color0, Texture2D::create(
		newWidth, newHeight,
		TextureFormat::RGBA8,
		TextureFlag::RenderTarget | TextureFlag::ShaderResource
	));
	m_game.camera.setViewport(newWidth, newHeight);
}

Game& GameView::game()
{
	return m_game;
}

}