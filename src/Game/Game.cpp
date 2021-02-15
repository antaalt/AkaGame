#include "Game.h"

#include "../Component/Transform2D.h"
#include "../Component/Collider2D.h"
#include "../Component/Camera2D.h"
#include "../Component/Text.h"
#include "../Component/TileMap.h"
#include "../Component/TileLayer.h"
#include "../Component/Player.h"
#include "../Component/SoundInstance.h"
#include "../Component/Coin.h"
#include "../System/PhysicSystem.h"
#include "../System/AnimatorSystem.h"
#include "../System/TileMapSystem.h"
#include "../System/TileRenderSystem.h"
#include "../System/TextRenderSystem.h"
#include "../System/CameraSystem.h"
#include "../System/CollisionSystem.h"
#include "../System/PlayerSystem.h"
#include "../System/CoinSystem.h"
#include "../System/SoundSystem.h"
#include "../GUI/EntityWidget.h"
#include "../GUI/InfoWidget.h"
#include "../GUI/ResourcesWidget.h"
#include "../GUI/MenuWidget.h"
#include "OgmoWorld.h"


namespace aka {

Game::Game() :
	m_map(m_world)
{
}
void Game::initialize()
{
	Device device = Device::getDefault();
	Logger::info("Device vendor : ", device.vendor);
	Logger::info("Device renderer : ", device.renderer);
	Logger::info("Device memory : ", device.memory);
	Logger::info("Device version : ", device.version);

	Sampler sampler;
	sampler.filterMag = aka::Sampler::Filter::Nearest;
	sampler.filterMin = aka::Sampler::Filter::Nearest;
	sampler.wrapS = aka::Sampler::Wrap::Clamp;
	sampler.wrapT = aka::Sampler::Wrap::Clamp;
	{
		// INIT SYSTEMS
		m_world.attach<PhysicSystem>();
		m_world.attach<CollisionSystem>();
		m_world.attach<TileSystem>();
		m_world.attach<TileMapSystem>();
		m_world.attach<AnimatorSystem>();
		m_world.attach<CameraSystem>();
		m_world.attach<TextRenderSystem>();
		m_world.attach<PlayerSystem>();
		m_world.attach<CoinSystem>();
		m_world.attach<SoundSystem>();
		m_world.create();
	}

	{
		// INIT sounds
		Entity e = m_world.createEntity("BackgroundMusic");
		AudioStream::Ptr audio = AudioManager::create("Forest", AudioStream::openStream(Asset::path("sounds/forest.mp3")));
		e.add<SoundInstance>(SoundInstance(audio, true));
	}

	{
		// INIT FRAMEBUFFER
		m_framebuffer = Framebuffer::create(320, 180, sampler);
		PlatformBackend::setLimits(m_framebuffer->width(), m_framebuffer->height(), 0, 0);
	}

	{
		// INIT CAMERA
		m_cameraEntity = m_world.createEntity("Camera");
		m_cameraEntity.add<Transform2D>(Transform2D());
		m_cameraEntity.add<Camera2D>(Camera2D(vec2f(0), vec2f(320, 180) ));
	}

	{
		// INIT FONTS
		FontManager::create("Espera48", Font(Asset::path("font/Espera/Espera-Bold.ttf"), 48));
		FontManager::create("Espera16", Font(Asset::path("font/Espera/Espera-Bold.ttf"), 16));
		Font &font = FontManager::create("BoldFont48", Font(Asset::path("font/Theboldfont/theboldfont.ttf"), 48));
		Entity e = m_world.createEntity("Text");
		e.add<Transform2D>(Transform2D());
		e.add<Text>(Text());

		Text& text = e.get<Text>();
		text.offset = vec2f(0.f);
		text.color = color4f(1.f);
		text.font = &font;
		text.text = "Find them all !";
		text.layer = 2;
		vec2i size = font.size(text.text);
		Transform2D& transform = e.get<Transform2D>();
		transform.translate(vec2f((float)((int)m_framebuffer->width() / 2 - size.x / 2), (float)((int)m_framebuffer->height() / 2 - size.y / 2 - 50)));
	}

	{
		// INIT FIXED TEXTURE BACKGROUND
		Image image = Image::load(Asset::path("textures/background/background.png"));
		ASSERT(image.width == m_framebuffer->width(), "incorrect width");
		ASSERT(image.height == m_framebuffer->height(), "incorrect height");

		Sprite::Animation animation;
		animation.name = "Default";
		animation.frames.push_back(Sprite::Frame::create(Texture::create(image.width, image.height, Texture::Format::Rgba, image.bytes.data(), sampler), Time::Unit::milliseconds(500)));
		Sprite &sprite = SpriteManager::create("Background", Sprite());
		sprite.animations.push_back(animation);

		Entity e = m_world.createEntity("");
		e.add<Transform2D>(Transform2D());
		e.add<Animator>(Animator(&sprite, -2));
		e.get<Animator>().play("Default");
	}

	{
		m_currentLevel = "level0";
		m_map.loadLevel(m_currentLevel);
	}

	{
		// Initialize GUI
		m_gui.add<InfoWidget>();
		m_gui.add<EntityWidget>();
		m_gui.add<ResourcesWidget>();
		m_gui.add<MenuWidget>();
		m_gui.initialize();
	}
}

void Game::destroy()
{
	m_gui.destroy();
}

void Game::frame()
{
	m_gui.frame();
}
// Need a component movable -> take inputs and update movements
// Component hurtable
// collision -> if hurtable and collider has component damage
void Game::update(Time::Unit deltaTime)
{
	if (input::pressed(input::Key::F1))
	{
		GraphicBackend::screenshot("./output.jpg");
		Logger::info("Screenshot taken.");
	}
	if (!m_gui.focused() && input::down(input::Key::H))
	{
		m_gui.setVisible(!m_gui.isVisible());
	}
	// Update world after moving manually objects
	m_world.update(deltaTime);

	Level *level = m_map.getLevel(m_currentLevel);
	// TODO store player entity in Game instead of level
	auto view = m_world.registry().view<Player, Transform2D>();
	view.each([&](Player& player, Transform2D& transform) {
		vec2f pos = transform.position();
		vec2f size = transform.size();
		if (pos.x + size.x > level->width)
		{
			if (m_currentLevel != "level1")
			{
				m_map.deleteLevel("level0");
				m_map.loadLevel("level1");
				m_currentLevel = "level1";
			}
		}
		else if (pos.x < 0)
		{
			if (m_currentLevel != "level0")
			{
				m_map.deleteLevel("level1");
				m_map.loadLevel("level0");
				m_currentLevel = "level0";
			}
		}
		else if (pos.y + size.y > level->height)
		{

		}
		else if (pos.y < 0)
		{

		}
	});

	// Quit the app if requested
	if (input::pressed(aka::input::Key::Escape))
	{
		quit();
	}
	m_gui.update(m_world);
}

void Game::render()
{
	{
		// Render to framebuffer
		Camera2D& camera = m_cameraEntity.get<Camera2D>();
		mat4f view = mat4f::inverse(mat4f(
			col4f(1.f, 0.f, 0.f, 0.f),
			col4f(0.f, 1.f, 0.f, 0.f),
			col4f(0.f, 0.f, 1.f, 0.f),
			col4f(camera.position.x, camera.position.y, 0.f, 1.f)
		));
		mat4f projection = mat4f::orthographic(0.f, static_cast<float>(m_framebuffer->height()), 0.f, static_cast<float>(m_framebuffer->width()));
		m_framebuffer->clear(1.f, 0.63f, 0.f, 1.f); 
		m_world.draw(m_batch);
		m_batch.render(m_framebuffer, view, projection);
		m_drawCall = m_batch.count();
		m_batch.clear();
	}

	{
		// Blit to backbuffer
		GraphicBackend::backbuffer()->clear(0.f, 0.f, 0.f, 1.f);
		mat4f view = mat4f::identity();
		mat4f projection = mat4f::orthographic(0.f, static_cast<float>(height()), 0.f, static_cast<float>(width()));
		m_batch.draw(mat3f::scale(vec2f((float)width(), (float)height())), Batch::Rect(vec2f(0), vec2f(1.f), m_framebuffer->attachment(Framebuffer::AttachmentType::Color0), 0));
		m_batch.render(GraphicBackend::backbuffer(), view, projection);
		m_batch.clear();
	}

	{
		// Rendering imgui
		m_gui.draw(m_world);
		ImGui::ShowDemoWindow();
		m_gui.render();
	}

}

}