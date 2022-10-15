#pragma once

#include <Aka/Aka.h>
#include <Aka/Core/StrictType.h>
#include <Aka/Resource/Resource/Sprite.h>
#include <map>

#include "../Resources.h"

#include "../../System/CollisionSystem.h"

namespace aka {


struct PlayerDeathEvent {};
struct PlayerWinEvent {};

class GameView :
	public View,
	public EventListener<PlayerDeathEvent>
{
	struct Level;

	struct Player :
		WorldEventListener<CollisionEvent>
	{
		Player(World& world);
		void update(World& world, Time deltaTime);
		void receive(const CollisionEvent& event) override;
		Entity entity;
	};

	struct DeathWall
	{
		Entity entity;
		void update(Time deltaTime, Player& player);
	};

	struct Camera
	{
		void track(const Level& level, const Player& player);
		mat4f view() const;
		mat4f projection() const;
		void setViewport(uint32_t width, uint32_t height);

		Entity entity;
	};

	struct Level
	{
		using ID = StrictType<uint32_t, struct LevelTagName>;
		void load(ID levelID, World& world);
		void destroy();

		vec2f spawn;
		vec2f offset;
		vec2f size;
		std::vector<gfx::TextureHandle> textures;
		std::vector<AudioStream*> audios;
		std::vector<Sprite*> sprites;
		std::vector<Entity> entities;
	};

	struct Transition
	{
		Time startTime;
		Time duration;
		vec2f startPosition;
		vec2f endPosition;
		Level::ID previousLevelID;
		bool active;
	};

	struct Factory
	{
		static Entity player(World& world, const vec2f& position);
		static Entity background(World& world, const vec2f& position, const vec2f& size);
		static Entity wall(World& world, float height);
		static Entity camera(World& world, const vec2f& viewport);
		static Entity collider(World& world, const vec2f& position, const vec2f& size);
		static Entity coin(World& world, const vec2f& position, const vec2f& size);
		static Entity spike(World& world, const vec2f& position, const vec2f& size);
		static Entity layer(World& world, const vec2f& position, const vec2u& tileCount, const vec2u& atlasTileCount, const vec2u& tileSize, gfx::TextureHandle atlas, const std::vector<int>& data, int32_t layer);
		static Entity leave(World& world, const vec2f& pos, const vec2f& size, const color4f& color);
	};

public:
	GameView();
	// Init the game
	void onCreate() override;
	// Destroy the game
	void onDestroy() override;
	// Update the game logic at fixed timestep
	void onFixedUpdate(Time deltaTime) override;
	// Update the game logic
	void onUpdate(Time deltaTime) override;
	// Draw the game logic
	void onRender(gfx::Frame* frame) override;
	// Draw the game logic
	void onPresent() override;
	// Resize the game logic
	void onResize(uint32_t width, uint32_t height) override;
	// Receive player death events
	void onReceive(const PlayerDeathEvent& event) override;
	// Load a level, store it and make it current.
	void load(Level::ID level);
	// Destroy a level.
	void destroy(Level::ID level);
	// Save progress
	void save(const Path& path);
	// Load progress save
	void load(const Path& path);

	// Level
	World world;
	bool deathAnimation = false;
	Time deathAnimationStart;
	vec2f deathAnimationPos;
	Transition transition;
	Level* currentLevel;
	Level::ID currentLevelID;
	std::map<Level::ID, Level> levels;
	// Elements
	Camera camera;
	Player player;
	DeathWall wall;
};

}

