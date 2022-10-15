#include "IntroView.h"

#include "MenuView.h"

#include <Aka/Resource/Resource/Sprite.h>

namespace aka {

void IntroView::onCreate()
{
	/*Image image = Image::load(OS::cwd() + "asset/logo/aka.png");
	Sprite sprite;
	sprite.animations.emplace_back();
	SpriteAnimation& animation = sprite.animations.back();
	animation.name = "Default";
	gfx::TextureHandle texture = gfx::Texture::create2D(
		image.width(), 
		image.height(), 
		gfx::TextureFormat::RGBA8,
		gfx::TextureFlag::ShaderResource, 
		image.data()
	);
	animation.frames.push_back(Sprite::Frame::create(
		texture,
		image.width(), image.height(),
		Time::milliseconds(0)
	));

	SpriteManager::create("Logo", std::move(sprite));*/
	m_elapsed = Time::milliseconds(0);
}

void IntroView::onDestroy()
{
	//SpriteManager::destroy("Logo");
}

void IntroView::onUpdate(Time dt)
{
	Time fadeInDuration = Time::milliseconds(1000);
	Time stillDuration = Time::milliseconds(3000);
	Time fadeOutDuration = Time::milliseconds(1000);
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
		Time t = m_elapsed - (fadeInDuration + stillDuration);
		m_logoAlpha = 1.f - t.seconds() / fadeOutDuration.seconds();
	}
	else
	{
		EventDispatcher<ViewChangedEvent>::emit(ViewChangedEvent{View::create<MenuView>()});
	}
	m_elapsed += dt;
}

void IntroView::onRender(gfx::Frame* frame)
{
	/*gfx::FramebufferHandle backbuffer = Application::app()->graphic()->backbuffer(frame);
	backbuffer->clear(color4f(0.01f, 0.01f, 0.01f, 1.f));
	Renderer2D::clear();
	{
		Font::Ptr font = FontManager::getDefault();
		std::string txt = "Made with Aka engine";
		vec2i size = font->size(txt);
		mat3f transformText = mat3f::translate(vec2f(
			(float)((int)backbuffer->width() / 2 - size.x / 2),
			(float)((int)backbuffer->height() / 2 - size.y / 2) - 150.f
		));
		Renderer2D::drawText(transformText, txt, *font, color4f(1.f, 1.f, 1.f, m_logoAlpha), 0);
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
	Renderer2D::render();*/
}

};
