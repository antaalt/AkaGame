#include "GameView.h"

namespace aka {

void GameView::onCreate()
{
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
}

void GameView::onDestroy()
{
	m_game.destroy();
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
		GraphicBackend::backbuffer()->blit(m_framebuffer, FramebufferAttachmentType::Color0, Sampler::Filter::Nearest);
	}
}

void GameView::onResize(uint32_t width, uint32_t height)
{
	uint32_t newWidth = (uint32_t)(width * Game::resolution.y / (float)height);
	uint32_t newHeight = Game::resolution.y;

	m_framebuffer->resize(newWidth, newHeight);
	m_game.camera.setViewport(newWidth, newHeight);
}

Game& GameView::game()
{
	return m_game;
}

}