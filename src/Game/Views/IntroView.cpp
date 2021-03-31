#include "IntroView.h"

#include "MenuView.h"

namespace aka {

void IntroView::onCreate()
{
	{
		// INIT fonts
		FontManager::create("Espera48", Font(Asset::path("font/Espera/Espera-Bold.ttf"), 48));
		FontManager::create("Espera16", Font(Asset::path("font/Espera/Espera-Bold.ttf"), 16));
		FontManager::create("BoldFont48", Font(Asset::path("font/Theboldfont/theboldfont.ttf"), 48));
	}
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
}

void IntroView::onDestroy()
{
	{
		FontManager::destroy("Espera48");
		FontManager::destroy("Espera16");
		FontManager::destroy("BoldFont48");
	}
	SpriteManager::destroy("Logo");
}

void IntroView::onUpdate(Time::Unit dt)
{
	Time::Unit fadeInDuration = Time::Unit::milliseconds(1000);
	Time::Unit stillDuration = Time::Unit::milliseconds(3000);
	Time::Unit fadeOutDuration = Time::Unit::milliseconds(1000);
	if (m_elapsed <= fadeInDuration)
	{
		m_logoAlpha = m_elapsed.seconds() / fadeInDuration.seconds();
	}
	else if (m_elapsed > fadeInDuration && m_elapsed <= fadeInDuration + stillDuration)
	{
		m_logoAlpha = 1.f;
	}
	else if (m_elapsed > fadeInDuration + stillDuration && m_elapsed < fadeInDuration + stillDuration + fadeOutDuration)
	{
		Time::Unit t = m_elapsed - (fadeInDuration + stillDuration);
		m_logoAlpha = 1.f - t.seconds() / fadeOutDuration.seconds();
	}
	else
	{
		EventDispatcher<ViewChangedEvent>::emit(ViewChangedEvent{View::create<MenuView>()});
	}
	m_elapsed += dt;
}

void IntroView::onRender()
{
	Framebuffer::Ptr backbuffer = GraphicBackend::backbuffer();
	backbuffer->clear(color4f(0.01f, 0.01f, 0.01f, 1.f));
	Renderer2D::clear();
	{
		Font& font = FontManager::getDefault();
		std::string txt = "Made with Aka engine";
		vec2i size = font.size(txt);
		mat3f transformText = mat3f::translate(vec2f(
			(float)((int)backbuffer->width() / 2 - size.x / 2),
			(float)((int)backbuffer->height() / 2 - size.y / 2) - 150.f
		));
		Renderer2D::drawText(transformText, txt, &font, color4f(1.f, 1.f, 1.f, m_logoAlpha), 0);
	}
	{
		Sprite& sprite = SpriteManager::get("Logo");
		const Sprite::Frame& frame = sprite.getFrame(0, 0);
		vec2f size = vec2f(frame.width, frame.height) * 3.f;
		mat3f transformLogo = mat3f::translate(vec2f(
			(float)((int)backbuffer->width() / 2 - size.x / 2),
			(float)((int)backbuffer->height() / 2 - size.y / 2)
		));
		Renderer2D::drawRect(transformLogo, vec2f(0.f), size, uv2f(0.f), uv2f(1.f), frame.texture, color4f(1.f, 1.f, 1.f, m_logoAlpha), 1);
	}
	Renderer2D::render();
}

};