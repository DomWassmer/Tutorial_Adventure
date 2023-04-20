#pragma once

#include <vector>

#include "Player.h"
#include "Character.h"
#include "Tile.h"
#include "Object.h"
#include "UI.h"

// both width and length
#define CELL_SIZE 16

struct Cell {
	std::vector<Character> m_enemies;
	std::vector<Tile> m_staticTiles;
	std::vector<DynamicTile> m_dynamicTiles;
	std::vector<Object> m_objects;
	std::vector<DynamicObject> m_dynamicObjects;
};

class Scene {
public:
	enum class SceneType {
		MainMenu = 0, Level1
	};

public:
	Player m_player;
	std::vector<Cell> m_cellGrid;
	UI m_ui;

	[[nodiscard]] static std::shared_ptr<Scene> generateScene(SceneType sceneType);
	
	void printCellInfo(int cellNumber); 

private:
	Scene() = default;
	void generateScene_MainMenu();
	void generateScene_Level1();
};