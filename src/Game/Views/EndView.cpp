#include "EndView.h"

#include "MenuView.h"

namespace aka {

void EndView::onCreate()
{
	{
		// INIT fonts
		FontManager::create("Espera48", Font(Asset::path("font/Espera/Espera-Bold.ttf"), 48));
		FontManager::create("Espera16", Font(Asset::path("font/Espera/Espera-Bold.ttf"), 16));
		FontManager::create("BoldFont48", Font(Asset::path("font/Theboldfont/theboldfont.ttf"), 48));
	}
}

void EndView::onDestroy()
{
	{
		FontManager::destroy("Espera48");
		FontManager::destroy("Espera16");
		FontManager::destroy("BoldFont48");
	}
}

void EndView::onUpdate(Time::Unit dt)
{
	if (Keyboard::pressed(KeyboardKey::Space))
		EventDispatcher<ViewChangedEvent>::emit(ViewChangedEvent{ View::create<MenuView>() });
}

void EndView::onRender()
{
	Framebuffer::Ptr backbuffer = GraphicBackend::backbuffer();
	backbuffer->clear(color4f(0.01f, 0.01f, 0.01f, 1.f));
	{
		Font& font = FontManager::getDefault();
		std::string txt = "You won, congratulation !";
		vec2i size = font.size(txt);
		mat3f transformText = mat3f::translate(vec2f(
			(float)((int)backbuffer->width() / 2 - size.x / 2),
			(float)((int)backbuffer->height() / 2 - size.y / 2)
		));
		Renderer2D::drawText(transformText, txt, &font, color4f(1.f), 0);
	}
	{
		Font& font = FontManager::get("Espera16");
		std::string txt = "Press space to return to menu...";
		vec2i size = font.size(txt);
		mat3f transformText = mat3f::translate(vec2f(
			(float)((int)backbuffer->width() / 2 - size.x / 2),
			(float)((int)backbuffer->height() / 2 - size.y / 2) - 150.f
		));
		Renderer2D::drawText(transformText, txt, &font, color4f(1.f), 0);
	}
	Renderer2D::render();
	Renderer2D::clear();
}

};