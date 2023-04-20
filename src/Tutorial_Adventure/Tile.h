#pragma once

class Tile {
public:
	int m_gridLocation[3];
	int m_orientation[3];
	int m_rotation = 0; // modulo 4 rotation values
	int m_spriteIndex = -1;
	bool solid = true;
};

class DynamicTile : public Tile {};