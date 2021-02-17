#include "EndView.h"

namespace aka {

void EndView::onCreate()
{

}

void EndView::onDestroy()
{
}

void EndView::onUpdate(Router& router, Time::Unit dt)
{
}

void EndView::onRender()
{
	Framebuffer::Ptr backbuffer = GraphicBackend::backbuffer();
	backbuffer->clear(0.01f, 0.01f, 0.01f, 1.f);
	{
		Font& font = FontManager::getDefault();
		std::string txt = "You won, congratulation !";
		vec2i size = font.size(txt);
		mat3f transformText = mat3f::translate(vec2f(
			(float)((int)backbuffer->width() / 2 - size.x / 2),
			(float)((int)backbuffer->height() / 2 - size.y / 2)
		));
		m_batch.draw(transformText, Batch::Text(txt, &font, color4f(1.f), 0));
	}
	{
		Font& font = FontManager::get("Espera16");
		std::string txt = "Press escape to exit...";
		vec2i size = font.size(txt);
		mat3f transformText = mat3f::translate(vec2f(
			(float)((int)backbuffer->width() / 2 - size.x / 2),
			(float)((int)backbuffer->height() / 2 - size.y / 2) - 150.f
		));
		m_batch.draw(transformText, Batch::Text(txt, &font, color4f(1.f), 0));
	}
	m_batch.render(
		backbuffer,
		mat4f::identity(),
		mat4f::orthographic(0.f, (float)backbuffer->height(), 0.f, (float)backbuffer->width())
	);
	m_batch.clear();
}

};