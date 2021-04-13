#include "MenuView.h"

#include "../../Component/Transform2D.h"
#include "./GameView.h"

namespace aka {

void MenuView::onCreate()
{
	Font* font = &FontManager::getDefault();
	float padding = 10.f;
	vec2f center = vec2f(GraphicBackend::backbuffer()->width(), GraphicBackend::backbuffer()->height()) / 2.f;
	color4f red = color4f(0.93f, 0.04f, 0.26f, 1.f);
	color4f blue = color4f(0.01f, 0.47f, 0.96f, 1.f);
	color4f dark = color4f(0.1f, 0.1f, 0.1f, 1.f);
	color4f light = color4f(0.9f, 0.9f, 0.9f, 1.f);

	{
		Image img = Image::load(Asset::path("textures/background/background.png"));

		Entity e = m_world.createEntity("Background");
		e.add<Transform2DComponent>(Transform2DComponent(vec2f(0.f), vec2f(GraphicBackend::backbuffer()->width(), GraphicBackend::backbuffer()->height()), anglef::radian(0.f)));
		e.add<UIImageComponent>(UIImageComponent());
		UIImageComponent& image = e.get<UIImageComponent>();
		image.texture = Texture::create(img.width, img.height, TextureFormat::UnsignedByte, TextureComponent::RGBA, TextureFlag::None, Sampler::nearest());
		image.texture->upload(img.bytes.data());
		image.layer = 0;
	}

	{
		std::string text = "Start the game !!!"; // Collect 10 coins to win !
		vec2f s = vec2f(font->size(text)) + vec2f(padding) * 2.f;

		Entity e = m_world.createEntity("StartButton");
		e.add<Transform2DComponent>(Transform2DComponent(center - s / 2.f, vec2f(1.f), anglef::radian(0.f)));
		e.add<UIButtonComponent>(UIButtonComponent{
			font,
			text,
			padding,
			color4f(1),
			red,
			blue,
			lerp(red, blue, 0.3f),
			1,
			false,
			false,
			[&](const Position&) {
				EventDispatcher<ViewChangedEvent>::emit(ViewChangedEvent{ View::create<GameView>()});
			}
		});
	}

	{
		std::string text = "Config";
		vec2f s = vec2f(font->size(text)) + vec2f(padding) * 2.f;

		Entity e = m_world.createEntity("ConfigButton");
		e.add<Transform2DComponent>(Transform2DComponent(center - s / 2.f, vec2f(1.f), anglef::radian(0.f)));
		e.get<Transform2DComponent>().position.y -= 60.f;
		e.add<UIButtonComponent>(UIButtonComponent{
			font,
			text,
			padding,
			color4f(1),
			red,
			blue,
			lerp(red, blue, 0.3f),
			1,
			false,
			false,
			[&](const Position&) {
				Logger::info("Woah, config not implemented !");
			}
		});
	}
	{
		std::string text = "Quit";
		vec2f s = vec2f(font->size(text)) + vec2f(padding) * 2.f;

		Entity e = m_world.createEntity("QuitButton");
		e.add<Transform2DComponent>(Transform2DComponent(center - s / 2.f, vec2f(1.f), anglef::radian(0.f)));
		e.get<Transform2DComponent>().position.y -= 120.f;
		e.add<UIButtonComponent>(UIButtonComponent{
			font,
			text,
			padding,
			color4f(1),
			red,
			blue,
			lerp(red, blue, 0.3f),
			1,
			false,
			false,
			[&](const Position&) {
				EventDispatcher<QuitEvent>::emit(QuitEvent());
			}
		});
	}

	m_world.attach<UISystem>();
	m_world.create();
}

void MenuView::onDestroy()
{
	m_world.destroy();
}

void MenuView::onUpdate(Time::Unit dt)
{
	m_world.update(dt);
}

void MenuView::onRender()
{
	Framebuffer::Ptr backbuffer = GraphicBackend::backbuffer();
	backbuffer->clear(color4f(0.01f, 0.01f, 0.01f, 1.f));

	m_world.draw();

	Renderer2D::render();
	Renderer2D::clear();
}

void MenuView::onResize(uint32_t width, uint32_t height)
{
	// TODO just loop button and scale position ?
	onDestroy();
	onCreate();
}

void UISystem::update(World& world, Time::Unit deltaTime)
{
	auto viewButton = world.registry().view<Transform2DComponent, UIButtonComponent>();
	viewButton.each([](Transform2DComponent& transform, UIButtonComponent& button) {
		vec2f s = vec2f(button.font->size(button.text));
		const Position& mouse = Mouse::position();
		// Hovered
		if ((mouse.x > transform.position.x) && (mouse.x < transform.position.x + s.x + 2 * button.padding) && (mouse.y > transform.position.y) && (mouse.y < transform.position.y + s.y + 2 * button.padding))
			button.hovered = true;
		else
			button.hovered = false;
		// Active
		if (button.hovered && Mouse::pressed(MouseButton::ButtonLeft))
		{
			button.active = true;
			button.callback(mouse);
		}
		else
			button.active = false;
	});
}

void UISystem::draw(World& world)
{
	auto viewButton = world.registry().view<Transform2DComponent, UIButtonComponent>();
	viewButton.each([](Transform2DComponent& transform, UIButtonComponent& button) {
		vec2f s = vec2f(button.font->size(button.text));

		mat3f backgroundTransform = transform.model() * mat3f::scale(vec2f(s.x + 2 * button.padding, s.y + 2 * button.padding));
		mat3f textTransform = transform.model() * mat3f::translate(vec2f(button.padding, button.padding));

		Renderer2D::drawRect(backgroundTransform, vec2f(0.f), vec2f(1), uv2f(0.f), uv2f(1.f), nullptr, button.active ? button.colorButtonActive : (button.hovered ? button.colorButtonHovered : button.colorButton), button.layer);
		Renderer2D::drawText(textTransform, button.text, *button.font, button.colorText, button.layer + 1);
	});

	auto viewImage = world.registry().view<Transform2DComponent, UIImageComponent>();
	viewImage.each([&](Transform2DComponent& transform, UIImageComponent& image) {
		Renderer2D::drawRect(transform.model(), vec2f(0.f), vec2f(1.f), uv2f(0.f), uv2f(1.f), image.texture, color4f(1.f), image.layer);
	});
}

};