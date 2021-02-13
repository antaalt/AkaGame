#include "Level.h"

#include "OgmoWorld.h"
#include "../Component/Transform2D.h"
#include "../Component/TileMap.h"
#include "../Component/TileLayer.h"
#include "../Component/Collider2D.h"
#include "../Component/Animator.h"
#include "../Component/Coin.h"
#include "../Component/Text.h"
#include "../Component/Player.h"
#include "../Component/Door.h"
#include "../System/SoundSystem.h"
#include <cstddef>
#include <cstring>


#include <Aka/Core/Sprite.h>
#include <Aka/Scene/World.h>

namespace aka {

WorldMap::WorldMap(World& world, Resources& resources) :
	m_world(world),
	m_resources(resources),
	m_ogmoWorld(OgmoWorld::load(Asset::path("levels/world.ogmo")))
{
}

Level* WorldMap::getLevel(const std::string& level)
{
	auto it = m_levels.find(level);
	if (it == m_levels.end())
		return nullptr;
	return it->second;
}

void WorldMap::deleteLevel(const std::string& levelName)
{
	Level* level = getLevel(levelName);
	if (level != nullptr)
	{
		for (Entity& e : level->entities)
			if(e.valid())
				e.destroy();
		delete level;
		m_levels.erase(levelName);
	}
}

void WorldMap::loadLevel(const std::string &str)
{
	Level* level = new Level;
	auto it = m_levels.insert(std::make_pair(str, level));

	// Load Ogmo level
	Path path = Asset::path("levels/" + str + ".json");
	OgmoLevel ogmoLevel = OgmoLevel::load(m_ogmoWorld, path);
	const OgmoLevel::Layer* foreground = ogmoLevel.getLayer("Foreground");

	// - Size
	//ogmoLevel.offset;
	level->width = ogmoLevel.size.x;
	level->height = ogmoLevel.size.y;

	// - Layers
	Sampler sampler;
	sampler.filterMag = aka::Sampler::Filter::Nearest;
	sampler.filterMin = aka::Sampler::Filter::Nearest;
	sampler.wrapS = aka::Sampler::Wrap::Clamp;
	sampler.wrapT = aka::Sampler::Wrap::Clamp;
	std::map<std::string, Texture::Ptr> atlas;
	auto createTileLayer = [&](Level::Layer& layer, const OgmoLevel::Layer* ogmoLayer, int32_t layerDepth) -> Entity {
		if (ogmoLayer->layer->type != OgmoWorld::LayerType::Tile)
			return Entity::null();
		ASSERT(ogmoLayer->tileset->tileSize == ogmoLayer->gridCellSize, "");
		Texture::Ptr texture;
		auto it = atlas.find(ogmoLayer->tileset->name);
		if (it == atlas.end())
		{
			texture = Texture::create(
				ogmoLayer->tileset->image.width, 
				ogmoLayer->tileset->image.height, 
				Texture::Format::Rgba, 
				ogmoLayer->tileset->image.bytes.data(), 
				sampler
			);
			atlas.insert(std::make_pair(ogmoLayer->tileset->name, texture));
		}
		else
		{
			texture = it->second;
		}
		
		layer.atlas = texture;
		layer.tileID = ogmoLayer->data;

		Entity entity = m_world.createEntity("Layer");
		entity.add<Transform2D>(Transform2D(vec2f(0.f), vec2f(1.f), radianf(0.f)));
		entity.add<TileMap>(TileMap(ogmoLayer->tileset->tileCount, ogmoLayer->tileset->tileSize, texture));
		entity.add<TileLayer>(TileLayer(vec2f(0.f), ogmoLayer->gridCellCount, ogmoLayer->gridCellSize, color4f(1.f), ogmoLayer->data, layerDepth));
		return entity;
	};
	// - Layers
	level->entities.push_back(createTileLayer(level->background, ogmoLevel.getLayer("Background"), -1));
	level->entities.push_back(createTileLayer(level->playerGround, ogmoLevel.getLayer("Playerground"), 0));
	level->entities.push_back(createTileLayer(level->foreground, ogmoLevel.getLayer("Foreground"), 1));

	// - Background texture
	std::vector<uint8_t> data;
	data.resize(level->width * level->height * 4);
	memset(data.data(), 0xffffffff, sizeof(data.size()));
	level->backgroundTexture = Texture::create(level->width, level->height, Texture::Format::Rgba, data.data(), sampler);

	{
		// Audio effect
		Resources::audio.create("Jump", AudioStream::loadMemory(Asset::path("sounds/jump.mp3")));
	}

	{
		// Colliders
		m_resources.sprite.create("Collider", Sprite::parse(Asset::path("textures/debug/collider.aseprite")));
		// Coins
		m_resources.sprite.create("Coin", Sprite::parse(Asset::path("textures/interact/interact.aseprite")));
		// Player
		m_resources.sprite.create("Player", Sprite::parse(Asset::path("textures/player/player.aseprite")));
	}
	const OgmoLevel::Layer* layer = ogmoLevel.getLayer("Colliders");
	for (const OgmoLevel::Entity& entity : layer->entities)
	{
		if (entity.entity->name == "Collider")
		{
			Entity e = m_world.createEntity("Collider");
			e.add<Transform2D>(Transform2D(vec2f((float)entity.position.x, (float)(layer->getHeight() - entity.position.y - entity.size.y)), vec2f(entity.size) / 16.f, radianf(0.f)));
			e.add<Collider2D>(Collider2D(vec2f(0.f), vec2f(16.f)));
			//e.add<Animator>(Animator(m_sprites.back().get(), 1));
			level->entities.push_back(e);
		}
		else if (entity.entity->name == "Coin")
		{
			Entity e = m_world.createEntity("Coin");
			e.add<Transform2D>(Transform2D(vec2f((float)entity.position.x, (float)(layer->getHeight() - entity.position.y - entity.size.y)), vec2f(entity.size) / 16.f, radianf(0)));
			e.add<Collider2D>(Collider2D(vec2f(0.f), vec2f(16.f), CollisionType::Event, 0.1f, 0.1f));
			e.add<Animator>(Animator(m_resources.sprite.get("Coin"), 1));
			e.add<Coin>(Coin());
			e.get<Animator>().play("Idle");
			level->entities.push_back(e);
		}
		else if (entity.entity->name == "Character")
		{
			// TODO do not load player with level
			Sprite* playerSprite = m_resources.sprite.get("Player");
			Entity e = m_world.createEntity("Character");
			e.add<Transform2D>(Transform2D(vec2f(80, 224), vec2f(1.f), radianf(0)));
			e.add<Animator>(Animator(playerSprite, 1));
			e.add<RigidBody2D>(RigidBody2D(1.f));
			e.add<Collider2D>(Collider2D(vec2f(0.f), vec2f((float)playerSprite->animations[0].frames[0].width, (float)playerSprite->animations[0].frames[0].height), CollisionType::Solid, 0.1f, 0.1f));
			e.add<Player>(Player());

			e.get<Animator>().play("Idle");
			Player& player = e.get<Player>();
			player.jump = Control(input::Key::Space);
			player.left = Control(input::Key::Q);
			player.right = Control(input::Key::D);

			e.add<Text>(Text(vec2f(3.f, 17.f), m_resources.font.get("Espera16"), "0", color4f(1.f), 3));
			level->entities.push_back(e);
		}
		/*else if (entity.entity->name == "LevelDoor")
		{
			const vec2f pos = vec2f((float)entity.position.x, (float)(layer->getHeight() - entity.position.y - entity.size.y));
			const vec2f size = vec2f(entity.size);
			Entity e = m_world.createEntity("Door");
			e.add<Transform2D>(Transform2D(pos, size, radianf(0)));
			e.add<Collider2D>(Collider2D(vec2f(0.f), vec2f(1.f)));
			e.add<Door>(Door());
			level->entities.push_back(e);
			level->doors.emplace_back();
			level->doors.back().name = entity.entity->name; // TODO get level name from ogmo file
		}*/
		else
		{
			Logger::warn("Ogmo entity not supported : ", entity.entity->name);
		}
	}
}

};