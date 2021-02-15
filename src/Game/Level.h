#pragma once

#include "Resources.h"
#include "OgmoWorld.h"

#include <Aka/Scene/Entity.h>

#include <string>

namespace aka {

struct Level
{
	// Rendering layer
	struct Layer
	{
		Texture::Ptr atlas;
		std::vector<int> tileID;
	};
	vec2u spawn;
	vec2i offset;
	vec2u size;
	Layer foreground;
	Layer playerGround;
	Layer background;
	Texture::Ptr backgroundTexture;

	std::vector<Entity> entities;

	void load(const std::string& level, OgmoWorld &ogmoWorld, World& world);
	void destroy(World& world);
};

struct GridMap
{
	static constexpr size_t rowCount = 2;
	static constexpr size_t colCount = 3;

	using Row = std::array<std::string, colCount>;
	using Col = std::array<Row, rowCount>;

	// Get the number of columns in the grid
	size_t cols() const { return colCount; }
	// Get the number of rows in the grid
	size_t rows() const { return rowCount; }

	//Row& operator[](size_t index) { return m_grid[index]; }
	const Row &operator[](size_t index) const { return m_grid[index]; }

private:
	const Col m_grid = {{
		{{ "level0", "level1", "level2" }},
		{{ "level1", "level2", "level0"}}
	}};
};

class WorldMap
{
public:
	WorldMap();

	// Set the current level and retrieve it
	void set(uint32_t x, uint32_t y, World &world);
	// Set the next level by offseting the current one if there is one
	void next(int32_t xOffset, int32_t yOffset, World& world);
	// Get the current level
	Level& get();
private:
	// Get the level from the grid
	const std::string& getLevelFromGrid(uint32_t x, uint32_t y);
private:
	vec2u m_currentLevel;
	Level m_level;
	GridMap m_grid;
	OgmoWorld m_ogmoWorld;
};

}