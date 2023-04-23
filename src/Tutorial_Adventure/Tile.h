#pragma once

class Tile {
public:
	glm::vec3 m_gridLocation;
	glm::vec3 m_orientation;
	int m_rotation = 0; // modulo 4 rotation values
	int m_spriteIndex = -1;
	bool solid = true;
};

class DynamicTile : public Tile {};