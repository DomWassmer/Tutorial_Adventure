#pragma once

#include <vector>

#include "Player.h"
#include "Character.h"
#include "Tile.h"
#include "Object.h"
#include "UI.h"
#include "Camera.h"

#include <glm/glm.hpp>

// both width and length
#define CELL_SIZE 16

struct Cell {
	int cellPosition[2];
	std::vector<Character> m_enemies;
	std::vector<Tile> m_staticTiles;
	unsigned int m_staticTileModelRendererID = -1;
	unsigned int m_staticTileTextureRendererID = -1;
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
	Camera m_activeCamera;

	[[nodiscard]] static std::shared_ptr<Scene> generateScene(SceneType sceneType);
	
	void onUpdate();
	void printCellInfo(int cellNumber); 

private:
	Scene() = default;
	void generateScene_MainMenu();
	void generateScene_Level1();
};