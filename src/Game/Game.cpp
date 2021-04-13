#include "Game.h"

#include "Resources.h"

#include "../Component/Transform2D.h"
#include "../Component/TileMap.h"
#include "../Component/TileLayer.h"
#include "../Component/Collider2D.h"
#include "../Component/Camera2D.h"
#include "../Component/SpriteAnimator.h"
#include "../Component/Coin.h"
#include "../Component/Text.h"
#include "../Component/Player.h"
#include "../Component/Particle.h"
#include "../Component/SoundInstance.h"
#include "../Component/Hurtable.h"

#include "../System/PhysicSystem.h"
#include "../System/SpriteAnimatorSystem.h"
#include "../System/TileMapRenderSystem.h"
#include "../System/SpriteRenderSystem.h"
#include "../System/TextRenderSystem.h"
#include "../System/CollisionSystem.h"
#include "../System/CoinSystem.h"
#include "../System/SoundSystem.h"
#include "../System/ParticleSystem.h"

namespace aka {

Entity Game::Factory::player(World& world, const vec2f& position)
{
	FileStream fs(Asset::path("textures/player/player.aseprite"), FileMode::ReadOnly);
	SpriteManager::create("Player", Sprite::parse(fs));
	Sprite& playerSprite = SpriteManager::get("Player");
	const Sprite::Frame& frame = playerSprite.getFrame(0, 0);

	Entity e = world.createEntity("Character");
	e.add<Transform2DComponent>(Transform2DComponent(position, vec2f(1.f), anglef::radian(0)));
	e.add<SpriteAnimatorComponent>(SpriteAnimatorComponent(&playerSprite, 1));
	e.add<RigidBody2DComponent>(RigidBody2DComponent(0.2f));
	e.add<Collider2DComponent>(Collider2DComponent(vec2f(0.f), vec2f((float)frame.width, (float)frame.height), CollisionType::Solid, 0.1f, 0.1f));
	e.add<PlayerComponent>(PlayerComponent());

	e.get<SpriteAnimatorComponent>().play("Idle");
	PlayerComponent& player = e.get<PlayerComponent>();
	player.jump.add(KeyboardKey::Space);
	player.jump.add(GamepadButton::A);
	player.motion.add(KeyboardKey::A, KeyboardKey::D, KeyboardKey::W, KeyboardKey::S);
	player.motion.add(GamepadAxis::Left);

	e.add<Text2DComponent>(Text2DComponent(vec2f(3.f, 17.f), &FontManager::get("Espera16"), "0", color4f(1.f), 3));

	return e;
}

Entity Game::Factory::background(World& world, const vec2f& position, const vec2f& size, const Image& image)
{
	Sprite::Animation animation;
	animation.name = "Default";
	animation.frames.push_back(Sprite::Frame::create(
		Texture::create(image.width, image.height, TextureFormat::UnsignedByte, TextureComponent::RGBA, TextureFlag::None, Sampler{}),
		Time::Unit::milliseconds(500)
	));
	animation.frames.back().texture->upload(image.bytes.data());
	vec2f scale = vec2f(size) / vec2f(animation.frames.back().width, animation.frames.back().height);
	Sprite s;
	s.animations.push_back(animation);
	Sprite& sprite = SpriteManager::create("Background", std::move(s));

	Entity e = world.createEntity("Background");
	e.add<Transform2DComponent>(Transform2DComponent(Transform2DComponent(position, scale, anglef::radian(0.f))));
	e.add<SpriteAnimatorComponent>(SpriteAnimatorComponent(&sprite, -2));
	e.get<SpriteAnimatorComponent>().play("Default");
	return e;
}

Entity Game::Factory::wall(World& world, float height)
{
	// INIT DEATH WALL
	FileStream fs(Asset::path("textures/collider/collider.aseprite"), FileMode::ReadOnly);
	Sprite& colliderSprite = SpriteManager::create("Collider", Sprite::parse(fs));
	colliderSprite.animations[0].frames[0].width = 1;
	colliderSprite.animations[0].frames[0].height = 1;
	Entity e = world.createEntity("DeathWall");
	e.add<Transform2DComponent>(Transform2DComponent(vec2f(0.f), vec2f(10.f, height), anglef::radian(0.f)));
	e.add<SpriteAnimatorComponent>(SpriteAnimatorComponent(&colliderSprite, 5));
	e.add<Collider2DComponent>(Collider2DComponent(vec2f(0.f), vec2f(1.f), CollisionType::Event, 0.1f, 0.1f));
	e.add<HurtComponent>();
	return e;
}

Entity Game::Factory::camera(World& world, const vec2f& viewport)
{
	Entity e = world.createEntity("Camera");
	e.add<Transform2DComponent>(Transform2DComponent());
	e.add<Camera2DComponent>(Camera2DComponent(viewport));
	e.get<Camera2DComponent>().main = true;
	return e;
}

Entity Game::Factory::collider(World& world, const vec2f& position, const vec2f& size)
{
	Entity e = world.createEntity("Collider");
	e.add<Transform2DComponent>(Transform2DComponent(position, size, anglef::radian(0.f)));
	e.add<Collider2DComponent>(Collider2DComponent(vec2f(0.f), vec2f(16.f)));
	return e;
}

Entity Game::Factory::coin(World& world, const vec2f& position, const vec2f& size)
{
	Sprite& sprite = SpriteManager::get("Coin");
	Entity e = world.createEntity("Coin");
	e.add<Transform2DComponent>(Transform2DComponent(position, size, anglef::radian(0)));
	e.add<Collider2DComponent>(Collider2DComponent(vec2f(0.f), vec2f(16.f), CollisionType::Event, 0.1f, 0.1f));
	e.add<SpriteAnimatorComponent>(SpriteAnimatorComponent(&sprite, 1));
	e.add<CoinComponent>();
	e.get<SpriteAnimatorComponent>().play("Idle");
	return e;
}

Entity Game::Factory::spike(World& world, const vec2f& position, const vec2f& size)
{
	Entity e = world.createEntity("Spike");
	e.add<Transform2DComponent>(Transform2DComponent(position, size, anglef::radian(0)));
	e.add<Collider2DComponent>(Collider2DComponent(vec2f(0.f), vec2f(1.f), CollisionType::Event));
	e.add<HurtComponent>();
	return e;
}

Entity Game::Factory::layer(World& world, const vec2f& position, const vec2u& tileCount, const vec2u& atlasTileCount, const vec2u& tileSize, Texture::Ptr atlas, const std::vector<int>& data, int32_t layer)
{
	Entity entity = world.createEntity("Layer");
	entity.add<Transform2DComponent>(Transform2DComponent(vec2f(0.f), vec2f(1.f), anglef::radian(0.f)));
	entity.add<TileMapComponent>(TileMapComponent(atlasTileCount, tileSize, atlas));
	entity.add<TileLayerComponent>(TileLayerComponent(position, tileCount, tileSize, color4f(1.f), data, layer));
	return entity;
}

Entity Game::Factory::leave(World& world, const vec2f& pos, const vec2f& size, const color4f& color)
{
	Entity e = world.createEntity("Particle");
	// Generate random coordinates / rotation in level space ?
	e.add<Transform2DComponent>(Transform2DComponent(
		vec2f((float)pos.x + random<float>(0.f, (float)size.x), (float)pos.y + random<float>(0.f, (float)size.y)),
		vec2f(3.f),
		anglef::radian(random<float>(0.f, 2.f * pi<float>.radian()))
	));
	e.add<Particle2DComponent>(Particle2DComponent{ Time::now(), Time::zero(), vec2f::normalize(vec2f(1.f)), anglef::radian(0.f), vec2f(0.f), color, 10 });
	return e;
}

void Game::Level::load(ID levelID, World& world)
{
	// Load Ogmo level
	OgmoWorld ogmoWorld = OgmoWorld::load(Asset::path("levels/world.ogmo"));
	OgmoLevel ogmoLevel = OgmoLevel::load(ogmoWorld, Asset::path("levels/" + String::format("level%u", levelID.value()) + ".json"));

	// Size
	this->offset = vec2f(ogmoLevel.offset);
	this->size = vec2f(ogmoLevel.size);

	// Layers
	std::map<std::string, Texture::Ptr> atlas;
	auto createTileLayer = [&](const OgmoLevel::Layer* ogmoLayer, int32_t layerDepth) -> Entity {
		AKA_ASSERT(ogmoLayer->layer->type == OgmoWorld::LayerType::Tile, "");
		AKA_ASSERT(ogmoLayer->tileset->tileSize == ogmoLayer->gridCellSize, "");
		Texture::Ptr texture;
		auto it = atlas.find(ogmoLayer->tileset->name);
		if (it == atlas.end())
		{
			texture = Texture::create(
				ogmoLayer->tileset->image.width,
				ogmoLayer->tileset->image.height,
				TextureFormat::UnsignedByte,
				TextureComponent::RGBA,
				TextureFlag::None,
				Sampler{}
			);
			texture->upload(ogmoLayer->tileset->image.bytes.data());
			atlas.insert(std::make_pair(ogmoLayer->tileset->name, texture));
		}
		else
		{
			texture = it->second;
		}
		return Factory::layer(
			world,
			vec2f(offset), 
			ogmoLayer->gridCellCount,
			ogmoLayer->tileset->tileCount,
			ogmoLayer->tileset->tileSize, 
			texture, 
			ogmoLayer->data, 
			layerDepth
		);
	};
	
	// Layers
	this->entities.push_back(createTileLayer(ogmoLevel.getLayer("Background"), -1));
	this->entities.push_back(createTileLayer(ogmoLevel.getLayer("Playerground"), 0));
	this->entities.push_back(createTileLayer(ogmoLevel.getLayer("Foreground"), 1));
	
	// Background
	this->entities.push_back(Factory::background(world, offset, size, Image::load(Asset::path("textures/background/background.png"))));
		
	// Audio effect
	AudioManager::create("Jump", AudioStream::loadMemory(Asset::path("sounds/jump.mp3")));

	// Sprites
	FileStream fsCollider(Asset::path("textures/collider/collider.aseprite"), FileMode::ReadOnly);
	SpriteManager::create("Collider", Sprite::parse(fsCollider));
	FileStream fsCoin(Asset::path("textures/interact/interact.aseprite"), FileMode::ReadOnly);
	SpriteManager::create("Coin", Sprite::parse(fsCoin));

	// Entities
	const OgmoLevel::Layer* layer = ogmoLevel.getLayer("Entities");
	auto flipY = [](const vec2u& pos, const vec2u& size, const OgmoLevel::Layer* layer) -> vec2f {
		return vec2f((float)pos.x, (float)(layer->getHeight() - pos.y - size.y));
	};
	for (const OgmoLevel::Entity& entity : layer->entities)
	{
		if (entity.entity->name == "Collider")
		{
			this->entities.push_back(Factory::collider(
				world,
				vec2f(offset) + flipY(entity.position, entity.size, layer), 
				vec2f(entity.size) / 16.f
			));
		}
		else if (entity.entity->name == "Coin")
		{
			this->entities.push_back(Factory::coin(
				world,
				vec2f(offset) + flipY(entity.position, entity.size, layer),
				vec2f(entity.size) / 16.f
			));
		}
		else if (entity.entity->name == "Spawn")
		{
			this->spawn = vec2f(offset) + flipY(entity.position, entity.size, layer);
		}
		else if (entity.entity->name == "Spike")
		{
			this->entities.push_back(Factory::spike(
				world,
				vec2f(offset) + flipY(entity.position, entity.size, layer),
				vec2f(entity.size)
			));
		}
		else
		{
			Logger::warn("Ogmo entity not supported : ", entity.entity->name);
		}
	}

	{
		// Leaves
		float area = ogmoLevel.size.x / 16.f * ogmoLevel.size.y / 16.f;
		size_t particleCount = (size_t)(area / 10.f);
		color4f color = color4f(1.f, 0.75f, 0.80f, 1.f);

		for (size_t i = 0; i < particleCount; i++)
			this->entities.push_back(Factory::leave(world, vec2f(ogmoLevel.offset), vec2f(ogmoLevel.size), color));
	}
}

void Game::Level::destroy()
{
	for (Entity& e : entities)
		if (e.valid())
			e.destroy();
	textures.clear();
	audios.clear();
	sprites.clear();
	entities.clear();
	spawn = vec2f(0);
	size = vec2f(0);
	offset = vec2f(0);
}

Game::Game() :
	player(world)
{
}

void Game::initialize(uint32_t width, uint32_t height)
{
	// INIT SYSTEMS
	world.attach<PhysicSystem>();
	world.attach<CollisionSystem>();
	world.attach<SpriteRenderSystem>();
	world.attach<TileMapRenderSystem>();
	world.attach<SpriteAnimatorSystem>();
	world.attach<TextRenderSystem>();
	world.attach<CoinSystem>(world);
	world.attach<SoundSystem>();
	world.attach<ParticleSystem>();
	world.create();

	// INIT background sounds
	Entity e = world.createEntity("BackgroundMusic");
	AudioStream::Ptr audio = AudioManager::create("Forest", AudioStream::openStream(Asset::path("sounds/forest.mp3")));
	e.add<SoundInstance>(SoundInstance(audio, true));

	load(Level::ID(0));
	camera.entity = Game::Factory::camera(world, vec2f((float)width, (float)height));
	player.entity = Game::Factory::player(world, currentLevel->spawn);
	wall.entity = Game::Factory::wall(world, (float)height);
	transition = {};
}

void Game::destroy()
{
	EventDispatcher<PlayerDeathEvent>::clear();
	world.destroy();
}

void Game::update(Time::Unit deltaTime)
{
	EventDispatcher<PlayerDeathEvent>::dispatch();
	if (deathAnimation)
	{
		const Time::Unit duration = Time::Unit::milliseconds(500);
		Time::Unit now = Time::now();
		Time::Unit elapsed = now - deathAnimationStart;
		if (elapsed > duration)
		{
			Transform2DComponent& transform = player.entity.get<Transform2DComponent>();
			transform.position = currentLevel->spawn;
			wall.entity.get<Transform2DComponent>().size.x = currentLevel->offset.x - 50.f;
			deathAnimation = false;
			// Animation finished
		}
		else
		{
			// Shake screen
			Transform2DComponent& t = camera.entity.get<Transform2DComponent>();
			t.position.y = deathAnimationPos.y + cos(anglef::radian(now.seconds() * 2.f * pi<float>.radian() * 10.f)) * 2.f;
		}
	}
	else if (!transition.active)
	{
		// Only update world when not transitioning
		world.update(deltaTime);
		player.update(world, deltaTime);

		// Track camera
		camera.track(*currentLevel, player);

		// Move death wall
		//wall.update(deltaTime, player);

		// Check player position against level bounds
		PlayerComponent& playerComponent = player.entity.get<PlayerComponent>();
		Transform2DComponent& transformComponent = player.entity.get<Transform2DComponent>();
		Collider2DComponent& colliderComponent = player.entity.get<Collider2DComponent>();

		vec2f pos = transformComponent.model().multiplyPoint(colliderComponent.position);
		vec2f size = transformComponent.model().multiplyVector(colliderComponent.size);

		int32_t offset = 0;
		if (pos.x + size.x > currentLevel->size.x + currentLevel->offset.x)
		{
			offset = 1;
			transition.startPosition = camera.entity.get<Transform2DComponent>().position;
			transition.endPosition = vec2f(
				(float)(currentLevel->size.x + currentLevel->offset.x),
				transition.startPosition.y
			);
			Logger::info("Next level");
		}
		else if (pos.x + size.x < currentLevel->offset.x)
		{
			offset = -1;
			transition.startPosition = camera.entity.get<Transform2DComponent>().position;
			transition.endPosition = vec2f(
				(float)(currentLevel->offset.x - camera.entity.get<Camera2DComponent>().camera.viewport.x),
				transition.startPosition.y
			);
			Logger::info("Previous level");
		}
		/*else if (pos.y + size.y > currentLevel->size.y + currentLevel->offset.y)
		{
			offset = 1;
			transition.startPosition = camera.entity.get<Transform2DComponent>().position;
			transition.endPosition = vec2f(
				transition.startPosition.x,
				(float)(currentLevel->size.y + currentLevel->offset.y)
			);
			Logger::info("Next level");
		}
		else if (pos.y + size.y < currentLevel->offset.y)
		{
			offset = -1;
			transition.startPosition = camera.entity.get<Transform2DComponent>().position;
			transition.endPosition = vec2f(
				transition.startPosition.x,
				(float)(currentLevel->offset.y - camera.entity.get<Camera2DComponent>().camera.viewport.y)
			);
			Logger::info("Previous level");
		}*/
		if (offset != 0)
		{
			transition.active = true;
			transition.previousLevelID = currentLevelID;
			load(Level::ID(currentLevelID.value() + offset));
			transition.duration = Time::Unit::milliseconds(1000);
			transition.startTime = Time::now();
		}
	}
	else
	{
		float t = (Time::now() - transition.startTime).seconds() / transition.duration.seconds();
		if (t > 1.f)
		{
			// Transition finished. Destroy previous level.
			transition.active = false;
			destroy(transition.previousLevelID);
		}
		else
		{
			// Manage camera position
			camera.entity.get<Transform2DComponent>().position = lerp(transition.startPosition, transition.endPosition, t);
		}
	}
}

void Game::draw()
{
	world.draw();
}

void Game::onReceive(const PlayerDeathEvent& event)
{
	// TODO launch an animation and wait for the end.
	// Remove the rigid body component, launch animation
	// (player die and whole background become black. then restart.)
	// Restart 
	deathAnimation = true;
	deathAnimationStart = Time::now();
	deathAnimationPos = camera.entity.get<Transform2DComponent>().position;
}

void Game::load(Level::ID levelID)
{
	Level level;
	level.load(levelID, world);
	auto it = levels.insert(std::make_pair(levelID, level));
	if (!it.second)
		throw std::runtime_error("Failed to add level");
	currentLevel = &it.first->second;
	currentLevelID = levelID;
}

void Game::destroy(Level::ID level)
{
	auto it = levels.find(level);
	if (it == levels.end())
		return;
	it->second.destroy();
	levels.erase(it);
}

void Game::save(const Path& path)
{
	// Only save current level ?
	// And which coins where picked
}

void Game::load(const Path& path)
{
}

void Game::Camera::track(const Level& level, const Player& player)
{
	Transform2DComponent& cameraTransformComponent = entity.get<Transform2DComponent>();
	Camera2DComponent& cameraComponent = entity.get<Camera2DComponent>();
	CameraOrthographic& camera = cameraComponent.camera;
	// Track the player
	PlayerComponent& playerComponent = player.entity.get<PlayerComponent>();
	Transform2DComponent& playerTransformComponent = player.entity.get<Transform2DComponent>();

	const float hThreshold = 0.4f * camera.viewport.x;
	const float vThreshold = 0.4f * camera.viewport.y;
	const vec2f playerPosition = playerTransformComponent.position;
	const vec2f playerRelativePosition = playerPosition - cameraTransformComponent.position;

	if (level.size.x > level.size.y)
	{
		// Horizontal level
		if (playerRelativePosition.x < hThreshold)
		{
			float distance = abs<float>(playerRelativePosition.x - hThreshold);
			cameraTransformComponent.position.x = cameraTransformComponent.position.x - distance;
		}
		else if (playerRelativePosition.x > camera.viewport.x - hThreshold)
		{
			float distance = playerRelativePosition.x - (camera.viewport.x - hThreshold);
			cameraTransformComponent.position.x = cameraTransformComponent.position.x + distance;
		}
	}
	else
	{
		// Vertical level
		if (playerRelativePosition.y < vThreshold)
		{
			float distance = abs<float>(playerRelativePosition.y - vThreshold);
			cameraTransformComponent.position.y = cameraTransformComponent.position.y - distance;
		}
		else if (playerRelativePosition.y > camera.viewport.y - vThreshold)
		{
			float distance = playerRelativePosition.y - (camera.viewport.y - vThreshold);
			cameraTransformComponent.position.y = cameraTransformComponent.position.y + distance;
		}
	}

	// Clamp camera position to the current level bounds.
	vec2f grid = vec2f(level.size) + vec2f(level.offset);
	cameraTransformComponent.position.x = max(cameraTransformComponent.position.x, (float)level.offset.x);
	cameraTransformComponent.position.x = min(cameraTransformComponent.position.x, grid.x - camera.viewport.x);
	cameraTransformComponent.position.y = max(cameraTransformComponent.position.y, (float)level.offset.y);
	cameraTransformComponent.position.y = min(cameraTransformComponent.position.y, grid.y - camera.viewport.y);
}

mat4f Game::Camera::view() const
{
	const Transform2DComponent& cameraTransform = entity.get<Transform2DComponent>();
	return mat4f::inverse(mat4f::from2D(cameraTransform.model()));
}

mat4f Game::Camera::projection() const
{
	Camera2DComponent& camera = entity.get<Camera2DComponent>();
	return camera.camera.perspective();
}

void Game::Camera::setViewport(uint32_t width, uint32_t height)
{
	entity.get<Camera2DComponent>().camera.viewport = vec2f(width, height);
}

Game::Player::Player(World& world) :
	WorldEventListener<CollisionEvent>(world)
{
}

void emitJumpParticles(Transform2DComponent& transform, float velocity, World& world)
{
	color4f color = color4f(0.8f);
	Time::Unit duration = Time::Unit::milliseconds(500);
	Entity e = world.createEntity("Particle");
	e.add<Transform2DComponent>(Transform2DComponent(transform.position, vec2f(4.f), anglef::radian(0.f)));
	e.add<Particle2DComponent>(Particle2DComponent{ Time::now(), duration, vec2f::normalize(vec2f(0.f, -1.f)) * velocity, anglef::radian(random(6.f)), vec2f(5.f), color, 10 });

	e = world.createEntity("Particle");
	e.add<Transform2DComponent>(Transform2DComponent(transform.position, vec2f(4.f), anglef::radian(0.f)));
	e.add<Particle2DComponent>(Particle2DComponent{ Time::now(), duration, vec2f::normalize(vec2f(1.f, -1.f)) * velocity, anglef::radian(random(6.f)), vec2f(5.f), color, 10 });

	e = world.createEntity("Particle");
	e.add<Transform2DComponent>(Transform2DComponent(transform.position, vec2f(4.f), anglef::radian(0.f)));
	e.add<Particle2DComponent>(Particle2DComponent{ Time::now(), duration, vec2f::normalize(vec2f(-1.f, -1.f)) * velocity, anglef::radian(random(6.f)), vec2f(5.f), color, 10 });
}

void Game::Player::update(World& world, Time::Unit deltaTime)
{
	PlayerComponent& player = entity.get<PlayerComponent>();
	SpriteAnimatorComponent& animator = entity.get<SpriteAnimatorComponent>();
	RigidBody2DComponent& rigid = entity.get<RigidBody2DComponent>();
	Transform2DComponent& transform = entity.get<Transform2DComponent>();

	// Update inputs
	player.motion.update();
	player.jump.update();

	// Run
	const float runSpeed = player.speed;
	const float runFriction = 30.f;
	// Jump
	const float initialJumpVelocity = 16.f;
	const float initialDoubleJumpVelocity = 5.f;
	const float jumpLateralAcceleration = 20.f;
	const float jumpLateralFriction = 10.f;
	const float maxJumpLateralAcceleration = 10.f;

	if (player.state == PlayerComponent::State::Jumping || player.state == PlayerComponent::State::DoubleJumping)
	{
		const Position& delta = player.motion.delta();
		if (delta.x != 0.f)
		{
			animator.play("Run");
			animator.flipU = player.motion.delta().x < 0.f;
			rigid.velocity.x += delta.x * jumpLateralAcceleration * deltaTime.seconds();
			rigid.velocity.x = clamp(rigid.velocity.x, -maxJumpLateralAcceleration, maxJumpLateralAcceleration);
		}
		else if (rigid.velocity.x != 0.f)
		{
			// slow down until idle.
			if (rigid.velocity.x > 0.f)
				rigid.velocity.x = max(rigid.velocity.x - jumpLateralFriction * deltaTime.seconds(), 0.f);
			else
				rigid.velocity.x = min(rigid.velocity.x + jumpLateralFriction * deltaTime.seconds(), 0.f);
		}
		if (player.jump.down() && player.state == PlayerComponent::State::Jumping)
		{
			world.createEntity("DoubleJumpFX").add<SoundInstance>(SoundInstance(AudioManager::get("Jump"), 1.f));
			player.state = PlayerComponent::State::DoubleJumping;
			rigid.velocity.y = initialDoubleJumpVelocity;
			// Allow double jumping in the opposite direction.
			if (delta.x != 0.f && rigid.velocity.x > 0.f)
			{
				rigid.velocity.x = delta.x * initialDoubleJumpVelocity;
			}
			emitJumpParticles(transform, initialJumpVelocity, world);
		}
		// Stretch model depending on velocity
		const float limit = 10.f;
		transform.size.y = (clamp(rigid.velocity.y, -limit, limit) / limit) * -0.3f + 1.f; // [0.7, 1.3]
	}
	else
	{
		const Position& delta = player.motion.delta();
		transform.size.y = 1.f;
		if (delta.x != 0.f)
		{
			animator.play("Run");
			animator.flipU = player.motion.delta().x < 0.f;
			player.state = PlayerComponent::State::Walking;
			rigid.velocity.x = delta.x * runSpeed;
		}
		else if (rigid.velocity.x != 0.f)
		{
			// slow down until idle.
			if (rigid.velocity.x > 0.f)
				rigid.velocity.x = max(rigid.velocity.x - runFriction * deltaTime.seconds(), 0.f);
			else
				rigid.velocity.x = min(rigid.velocity.x + runFriction * deltaTime.seconds(), 0.f);
		}
		else if (animator.getCurrentSpriteAnimation().name != "Idle")
		{
			animator.play("Idle");
			player.state = PlayerComponent::State::Idle;
		}

		if (player.jump.down())
		{
			world.createEntity("JumpFX").add<SoundInstance>(SoundInstance(AudioManager::get("Jump"), 1.f));
			player.state = PlayerComponent::State::Jumping;
			rigid.velocity.y = initialJumpVelocity;
			emitJumpParticles(transform, initialJumpVelocity, world);
		}
	}
}

void Game::Player::receive(const CollisionEvent& event)
{
	Entity playerEntity = event.dynamicEntity;
	Entity staticEntity = event.staticEntity;
	if (!playerEntity.valid() || !staticEntity.valid() || !playerEntity.has<PlayerComponent>())
		return;
	PlayerComponent& player = playerEntity.get<PlayerComponent>();
	if (staticEntity.has<CoinComponent>())
	{
		CoinComponent& coin = staticEntity.get<CoinComponent>();
		if (!coin.picked)
		{
			coin.picked = true;
			player.coin++;
			if (staticEntity.has<SpriteAnimatorComponent>())
				staticEntity.get<SpriteAnimatorComponent>().play("Picked");
			if (playerEntity.has<Text2DComponent>())
				playerEntity.get<Text2DComponent>().text = std::to_string(player.coin);
		}
	}
	else if (staticEntity.has<HurtComponent>())
	{
		EventDispatcher<PlayerDeathEvent>::emit();
	}
	else if (CollisionFace::Bottom == event.face)
	{
		player.state = PlayerComponent::State::Idle;
		//playerEntity.get<Animator>().play("Idle");
	}
}

void Game::DeathWall::update(Time::Unit deltaTime, Player& player)
{
	float dist = player.entity.get<Transform2DComponent>().position.x - entity.get<Transform2DComponent>().size.x;
	float speed = max(deltaTime.seconds() * dist, deltaTime.seconds() * 16 * 2);
	entity.get<Transform2DComponent>().size.x += speed;
}

};