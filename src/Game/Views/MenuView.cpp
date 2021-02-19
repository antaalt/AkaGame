#include "MenuView.h"

namespace aka {

void MenuView::onCreate()
{
	Image image = Image::load(Asset::path("logo/aka.png"));
	Sprite sprite;
	sprite.animations.emplace_back();
	Sprite::Animation& animation = sprite.animations.back();
	animation.name = "Default";
	animation.frames.push_back(Sprite::Frame::create(
		Texture::create(image.width, image.height, Texture::Format::UnsignedByte, Texture::Component::RGBA, Sampler{}),
		Time::Unit::milliseconds(0))
	);
	animation.frames.back().texture->upload(image.bytes.data());

	SpriteManager::create("Logo", std::move(sprite));
	m_elapsed = Time::Unit::milliseconds(0);
	m_redraw = true;
}

void MenuView::onDestroy()
{
	SpriteManager::destroy("Logo");
}

void MenuView::onUpdate(Router& router, Time::Unit dt)
{
	Time::Unit fadeInDuration = Time::Unit::milliseconds(1000);
	Time::Unit stillDuration = Time::Unit::milliseconds(3000);
	Time::Unit fadeOutDuration = Time::Unit::milliseconds(1000);
	if (m_elapsed <= fadeInDuration)
	{
		m_redraw = true;
		m_logoAlpha = m_elapsed.seconds() / fadeInDuration.seconds();
	}
	else if (m_elapsed > fadeInDuration && m_elapsed <= fadeInDuration + stillDuration)
	{
		m_logoAlpha = 1.f;
		m_redraw = false;
	}
	else if (m_elapsed > fadeInDuration + stillDuration && m_elapsed < fadeInDuration + stillDuration + fadeOutDuration)
	{
		m_redraw = true;
		Time::Unit t = m_elapsed - (fadeInDuration + stillDuration);
		m_logoAlpha = 1.f - t.seconds() / fadeOutDuration.seconds();
	}
	else
	{
		router.set(Views::game);
	}
	m_elapsed += dt;
}

void MenuView::onRender()
{
	Framebuffer::Ptr backbuffer = GraphicBackend::backbuffer();
	backbuffer->clear(0.01f, 0.01f, 0.01f, 1.f);
	if (m_redraw)
	{
		m_batch.clear();
		{
			Font& font = FontManager::getDefault();
			std::string txt = "Collect 10 coins to win !";
			vec2i size = font.size(txt);
			mat3f transformText = mat3f::translate(vec2f(
				(float)((int)backbuffer->width() / 2 - size.x / 2),
				(float)((int)backbuffer->height() / 2 - size.y / 2) - 150.f
			));
			m_batch.draw(transformText, Batch::Text(txt, &font, color4f(1.f, 1.f, 1.f, m_logoAlpha), 0));
		}
		{
			Sprite& sprite = SpriteManager::get("Logo");
			const Sprite::Frame& frame = sprite.getFrame(0, 0);
			vec2f size = vec2f(frame.width, frame.height) * 3.f;
			mat3f transformLogo = mat3f::translate(vec2f(
				(float)((int)backbuffer->width() / 2 - size.x / 2),
				(float)((int)backbuffer->height() / 2 - size.y / 2)
			));
			m_batch.draw(transformLogo, Batch::Rect(vec2f(0.f), size, frame.texture, color4f(1.f, 1.f, 1.f, m_logoAlpha), 1));
		}
	}
	m_batch.render(
		backbuffer,
		mat4f::identity(), 
		mat4f::orthographic(0.f, (float)backbuffer->height(), 0.f, (float)backbuffer->width())
	);
}

};