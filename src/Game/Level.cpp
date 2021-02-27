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

WorldMap::WorldMap() :
	m_ogmoWorld(OgmoWorld::load(Asset::path("levels/world.ogmo")))
{
}

void WorldMap::set(uint32_t x, uint32_t y, World& world)
{
	Level level;
	level.load(getLevelFromGrid(x, y), m_ogmoWorld, world);
	m_currentLevel = vec2u(x, y);
	m_levels.insert(std::make_pair(m_currentLevel, level));
}

void WorldMap::next(int32_t xOffset, int32_t yOffset, World& world)
{
	ASSERT((m_currentLevel.x + xOffset) >= 0, "Integer overflow");
	ASSERT((m_currentLevel.y + yOffset) >= 0, "Integer overflow");
	uint32_t x = (m_currentLevel.x + xOffset) % m_grid.cols();
	uint32_t y = (m_currentLevel.y + yOffset) % m_grid.rows();
	set(x, y, world);
}

void WorldMap::destroy(uint32_t x, uint32_t y, World& world)
{
	auto it = m_levels.find(vec2u(x, y));
	if (it != m_levels.end())
	{
		it->second.destroy(world);
		m_levels.erase(it);
	}
}

Level& WorldMap::get()
{
	return m_levels.find(m_currentLevel)->second;
}

vec2u WorldMap::current() const
{
	return m_currentLevel;
}

const std::string& WorldMap::getLevelFromGrid(uint32_t x, uint32_t y)
{
	ASSERT(y < m_grid.rows(), "Not enough rows");
	ASSERT(x < m_grid.cols(), "Not enough cols");
	return m_grid[y][x];
}

void Level::load(const std::string& level, OgmoWorld& ogmoWorld, World& world)
{
	// Load Ogmo level
	Path path = Asset::path("levels/" + level + ".json");
	OgmoLevel ogmoLevel = OgmoLevel::load(ogmoWorld, path);
	const OgmoLevel::Layer* foreground = ogmoLevel.getLayer("Foreground");

	// - Size
	this->offset = ogmoLevel.offset;
	this->size = ogmoLevel.size;

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
				Texture::Format::UnsignedByte,
				Texture::Component::RGBA,
				sampler
			);
			texture->upload(ogmoLayer->tileset->image.bytes.data());
			atlas.insert(std::make_pair(ogmoLayer->tileset->name, texture));
		}
		else
		{
			texture = it->second;
		}

		layer.atlas = texture;
		layer.tileID = ogmoLayer->data;

		Entity entity = world.createEntity("Layer");
		entity.add<Transform2D>(Transform2D(vec2f(0.f), vec2f(1.f), radianf(0.f)));
		entity.add<TileMap>(TileMap(ogmoLayer->tileset->tileCount, ogmoLayer->tileset->tileSize, texture));
		entity.add<TileLayer>(TileLayer(vec2f(offset), ogmoLayer->gridCellCount, ogmoLayer->gridCellSize, color4f(1.f), ogmoLayer->data, layerDepth));
		return entity;
	};
	{
		// Layers
		this->entities.push_back(createTileLayer(this->background, ogmoLevel.getLayer("Background"), -1));
		this->entities.push_back(createTileLayer(this->playerGround, ogmoLevel.getLayer("Playerground"), 0));
		this->entities.push_back(createTileLayer(this->foreground, ogmoLevel.getLayer("Foreground"), 1));
	}

	{
		// Background
		Image image = Image::load(Asset::path("textures/background/background.png"));

		Sprite::Animation animation;
		animation.name = "Default";
		animation.frames.push_back(Sprite::Frame::create(
			Texture::create(image.width, image.height, Texture::Format::UnsignedByte, Texture::Component::RGBA, sampler),
			Time::Unit::milliseconds(500)
		));
		animation.frames.back().texture->upload(image.bytes.data());
		vec2f scale = vec2f(this->size) / vec2f(animation.frames.back().width, animation.frames.back().height);
		Sprite s;
		s.animations.push_back(animation);
		Sprite& sprite = SpriteManager::create("Background", std::move(s));

		Entity e = world.createEntity("Background");
		e.add<Transform2D>(Transform2D(Transform2D(vec2f(offset), scale, radianf(0.f))));
		e.add<Animator>(Animator(&sprite, -2));
		e.get<Animator>().play("Default");
		this->entities.push_back(e);
	}

	{
		// Audio effect
		AudioManager::create("Jump", AudioStream::loadMemory(Asset::path("sounds/jump.mp3")));
	}

	{
		// Colliders
		SpriteManager::create("Collider", Sprite::parse(FileStream(Asset::path("textures/collider/collider.aseprite"), FileMode::ReadOnly)));
		// Coins
		SpriteManager::create("Coin", Sprite::parse(FileStream(Asset::path("textures/interact/interact.aseprite"), FileMode::ReadOnly)));
		// Player
		SpriteManager::create("Player", Sprite::parse(FileStream(Asset::path("textures/player/player.aseprite"), FileMode::ReadOnly)));
	}
	const OgmoLevel::Layer* layer = ogmoLevel.getLayer("Entities");
	auto flipY = [](const vec2u& pos, const vec2u& size, const OgmoLevel::Layer *layer) -> vec2f {
		return vec2f((float)pos.x, (float)(layer->getHeight() - pos.y - size.y));
	};
	for (const OgmoLevel::Entity& entity : layer->entities)
	{
		if (entity.entity->name == "Collider")
		{
			Entity e = world.createEntity("Collider");
			e.add<Transform2D>(Transform2D(vec2f(offset) + flipY(entity.position, entity.size, layer), vec2f(entity.size) / 16.f, radianf(0.f)));
			e.add<Collider2D>(Collider2D(vec2f(0.f), vec2f(16.f)));
			this->entities.push_back(e);
		}
		else if (entity.entity->name == "Coin")
		{
			Entity e = world.createEntity("Coin");
			e.add<Transform2D>(Transform2D(vec2f(offset) + flipY(entity.position, entity.size, layer), vec2f(entity.size) / 16.f, radianf(0)));
			e.add<Collider2D>(Collider2D(vec2f(0.f), vec2f(16.f), CollisionType::Event, 0.1f, 0.1f));
			e.add<Animator>(Animator(&SpriteManager::get("Coin"), 1));
			e.add<Coin>(Coin());
			e.get<Animator>().play("Idle");
			this->entities.push_back(e);
		}
		else if (entity.entity->name == "Spawn")
		{
			this->spawn = vec2u(vec2f(offset) + flipY(entity.position, entity.size, layer));
		}
		else
		{
			Logger::warn("Ogmo entity not supported : ", entity.entity->name);
		}
	}
}

void Level::destroy(World& world)
{
	for (Entity& e : entities)
		if (e.valid())
			e.destroy();
	entities.clear();
	size = vec2u(0);
	offset = vec2i(0);
	foreground = Layer{};
	playerGround = Layer{};
	background = Layer{};
}

};