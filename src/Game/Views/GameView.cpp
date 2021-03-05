#include "GameView.h"

#include "../../Component/Transform2D.h"
#include "../../Component/Collider2D.h"
#include "../../Component/Camera2D.h"
#include "../../Component/Text.h"
#include "../../Component/TileMap.h"
#include "../../Component/TileLayer.h"
#include "../../Component/Player.h"
#include "../../Component/SoundInstance.h"
#include "../../Component/Coin.h"
#include "../../Component/Particle.h"
#include "../../System/PhysicSystem.h"
#include "../../System/AnimatorSystem.h"
#include "../../System/TileMapSystem.h"
#include "../../System/TileRenderSystem.h"
#include "../../System/TextRenderSystem.h"
#include "../../System/CameraSystem.h"
#include "../../System/CollisionSystem.h"
#include "../../System/PlayerSystem.h"
#include "../../System/CoinSystem.h"
#include "../../System/SoundSystem.h"
#include "../../System/LevelSystem.h"
#include "../../System/ParticleSystem.h"
#include "../Game.h"
#include "../OgmoWorld.h"


namespace aka {

static const uint32_t defaultWidth = 320;
static const uint32_t defaultHeight = 180;

GameView::GameView(World& world) :
	m_world(world),
	m_paused(false)
{
}
void GameView::onCreate()
{
	Device device = Device::getDefault();
	Logger::info("Device vendor : ", device.vendor);
	Logger::info("Device renderer : ", device.renderer);
	Logger::info("Device memory : ", device.memory);
	Logger::info("Device version : ", device.version);

	{
		// INIT SYSTEMS
		m_world.attach<PhysicSystem>();
		m_world.attach<CollisionSystem>();
		m_world.attach<TileSystem>();
		m_world.attach<TileMapSystem>();
		m_world.attach<AnimatorSystem>();
		m_world.attach<CameraSystem>(m_map);
		m_world.attach<TextRenderSystem>();
		m_world.attach<PlayerSystem>();
		m_world.attach<CoinSystem>();
		m_world.attach<SoundSystem>();
		m_world.attach<LevelSystem>(m_map);
		m_world.attach<ParticleSystem>();
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
		Sampler sampler;
		sampler.filterMag = aka::Sampler::Filter::Nearest;
		sampler.filterMin = aka::Sampler::Filter::Nearest;
		sampler.wrapS = aka::Sampler::Wrap::Clamp;
		sampler.wrapT = aka::Sampler::Wrap::Clamp;
		uint32_t width = (uint32_t)(GraphicBackend::backbuffer()->width() * defaultHeight / (float)GraphicBackend::backbuffer()->height());
		uint32_t height = defaultHeight;
		m_framebuffer = Framebuffer::create(width, height, sampler);
		PlatformBackend::setLimits(m_framebuffer->width(), m_framebuffer->height(), 0, 0);
	}

	{
		// INIT CAMERA
		m_cameraEntity = m_world.createEntity("Camera");
		m_cameraEntity.add<Transform2D>(Transform2D());
		m_cameraEntity.add<Camera2D>(Camera2D(vec2f((float)m_framebuffer->width(), (float)m_framebuffer->height())));
		m_cameraEntity.get<Camera2D>().main = true;
	}

	{
		// INIT LEVEL
		m_map.destroy(m_world);
		m_map.set(0, 0, m_world);
	}

	{
		// INIT CHARACTER
		Level& level = m_map.get();
		Sprite& playerSprite = SpriteManager::get("Player");
		const Sprite::Frame& frame = playerSprite.getFrame(0, 0);
		Entity e = m_world.createEntity("Character");
		e.add<Transform2D>(Transform2D(vec2f(level.spawn), vec2f(1.f), radianf(0)));
		e.add<Animator>(Animator(&playerSprite, 1));
		e.add<RigidBody2D>(RigidBody2D(0.2f));
		e.add<Collider2D>(Collider2D(vec2f(0.f), vec2f((float)frame.width, (float)frame.height), CollisionType::Solid, 0.1f, 0.1f));
		e.add<Player>(Player());

		e.get<Animator>().play("Idle");
		Player& player = e.get<Player>();
		player.jump = input::Key::Space;
		player.left = input::Key::A;
		player.right = input::Key::D;

		e.add<Text>(Text(vec2f(3.f, 17.f), &FontManager::get("Espera16"), "0", color4f(1.f), 3));
	}
}

void GameView::onDestroy()
{
	m_world.destroy();
}

void GameView::onFrame()
{
}

void GameView::onUpdate(Router &router, Time::Unit deltaTime)
{
	// Pause the GameView
	if (input::down(input::Key::P))
	{
		m_paused = !m_paused;
		if (m_paused)
		{
			m_cameraEntity.get<Camera2D>().clampBorder = false;
			AudioBackend::stop();
		}
		else
		{
			m_cameraEntity.get<Camera2D>().clampBorder = true;
			AudioBackend::start();
		}
	}
	// Update the GameView logic
	if (!m_paused)
	{
		// Update world after moving manually objects
		m_world.update(deltaTime);
	}

	// Reset
	if (input::down(input::Key::R))
	{
		vec2u current = m_map.current();
		m_map.set(0, 0, m_world);
		m_map.destroy(current.x, current.y, m_world);
		Level& level = m_map.get();
		auto view = m_world.registry().view<Transform2D, Player>();
		// TODO reinstantiate player ?
		view.each([&](Transform2D& transform, Player& player) {
			transform.position.x = (float)level.spawn.x;
			transform.position.y = (float)level.spawn.y;
		});
	}
	// Check if game finished
	/*auto view = m_world.registry().view<Player>();
	view.each([&](Player& player) {
		if (player.coin >= 10)
			router.set(Views::end);
	});*/
}

void GameView::onRender()
{
	{
		// Render to framebuffer
		Transform2D& cameraTransform = m_cameraEntity.get<Transform2D>();
		Camera2D& camera = m_cameraEntity.get<Camera2D>();
		mat4f view = mat4f::inverse(mat4f::from2D(cameraTransform.model()));
		mat4f projection = camera.camera.perspective();
		m_framebuffer->clear(1.f, 0.63f, 0.f, 1.f); 
		m_world.draw(m_batch);
		m_batch.render(m_framebuffer, view, projection);
		m_batch.clear();
	}
	
	{
		// Blit to backbuffer
		Framebuffer::Ptr backbuffer = GraphicBackend::backbuffer();
		backbuffer->clear(0.f, 0.f, 0.f, 1.f);
		m_batch.draw(mat3f::identity(), Batch::Rect(vec2f(0), vec2f(backbuffer->width(), backbuffer->height()), m_framebuffer->attachment(Framebuffer::AttachmentType::Color0), 0));
		m_batch.render();
		m_batch.clear();
	}

}

void GameView::onResize(uint32_t width, uint32_t height)
{
	uint32_t newWidth = (uint32_t)(width * defaultHeight / (float)height);
	uint32_t newHeight = defaultHeight;

	m_framebuffer->resize(newWidth, newHeight);
	m_cameraEntity.get<Camera2D>().camera.viewport = vec2f(newWidth, newHeight);
}

}