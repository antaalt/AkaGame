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
#include "../System/LevelSystem.h"
#include "../GUI/EntityWidget.h"
#include "../GUI/InfoWidget.h"
#include "../GUI/ResourcesWidget.h"
#include "../GUI/MenuWidget.h"
#include "OgmoWorld.h"


namespace aka {

Game::Game()
{
}
void Game::initialize()
{
	Logger::debug.mute();
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
		m_world.attach<LevelSystem>(m_map);
		m_world.create();
	}

	{
		// INIT background sounds
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
		// Set first level
		m_map.set(0, 0, m_world);
	}

	{
		Level& level = m_map.get();
		// Init character
		Sprite& playerSprite = SpriteManager::get("Player");
		const Sprite::Frame& frame = playerSprite.getFrame(0, 0);
		Entity e = m_world.createEntity("Character");
		e.add<Transform2D>(Transform2D(vec2f(level.spawn), vec2f(1.f), radianf(0)));
		e.add<Animator>(Animator(&playerSprite, 1));
		e.add<RigidBody2D>(RigidBody2D(1.f));
		e.add<Collider2D>(Collider2D(vec2f(0.f), vec2f((float)frame.width, (float)frame.height), CollisionType::Solid, 0.1f, 0.1f));
		e.add<Player>(Player());

		e.get<Animator>().play("Idle");
		Player& player = e.get<Player>();
		player.jump = Control(input::Key::Space);
		player.left = Control(input::Key::Q);
		player.right = Control(input::Key::D);

		e.add<Text>(Text(vec2f(3.f, 17.f), &FontManager::get("Espera16"), "0", color4f(1.f), 3));
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

	// Quit the app if requested
	if (input::down(input::Key::Escape))
	{
		quit();
	}
	// Reset
	if (input::down(input::Key::LeftCtrl))
	{
		m_map.set(0, 0, m_world);
		Level& level = m_map.get();
		auto view = m_world.registry().view<Transform2D, Player>();
		// TODO reinstantiate player ?
		view.each([&](Transform2D& transform, Player& player) {
			transform.model[2].x = level.spawn.x;
			transform.model[2].y = level.spawn.y;
		});
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
		m_gui.render();
	}

}

}