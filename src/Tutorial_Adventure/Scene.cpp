#include "Scene.h"

#include "Game.h"

#include <iostream>

std::shared_ptr<Scene> Scene::generateScene(SceneType sceneType)
{
	Scene scene;
	switch (sceneType)
	{
	case SceneType::MainMenu:
		scene.generateScene_MainMenu();
		break;
	case SceneType::Level1:
		scene.generateScene_Level1();
		break;
	default:
		throw std::runtime_error("Scene: Unknown Scene Type entered for scene generation!");
	}
	return std::make_shared<Scene>(scene);
}

void Scene::generateScene_MainMenu()
{
	return;
}

void Scene::generateScene_Level1() {
	{
		Game& gameInst = Game::getInstance();

		// For now only draw first sprite of character
		m_player.rendererID = gameInst.getRenderer3D()->requestID("Player");
		gameInst.getRenderer3D()->cmdLoadTexture(m_player.rendererID, "Walpurgia.png");
		m_player.m_position = glm::vec3(8.0f, 8.0f, 0.0f);
		m_player.m_spriteIndex = 0;

		m_activeCamera.setCameraHorizontalDistance(12.0f);
		m_activeCamera.setCameraHeight(6.0f);

		Cell cell_0;
		cell_0.cellPosition[0] = 0;
		cell_0.cellPosition[1] = 0;

		unsigned int tileRendererID = gameInst.getRenderer3D()->requestID("Floor Tiles");
		gameInst.getRenderer3D()->cmdLoadTexture(tileRendererID, "Sprite Floor Tiles.png");

		cell_0.m_staticTiles.resize(CELL_SIZE * CELL_SIZE);

		uint32_t spriteIndices[CELL_SIZE * CELL_SIZE] = 
		{ 
			3, 0, 0, 1, 1, 0, 2, 0, 0, 5, 2, 4, 0, 4, 3, 0, 
			0, 0, 0, 1, 0, 3, 2, 3, 1, 0, 2, 1, 0, 0, 2, 3, 
			4, 0, 1, 2, 0, 0, 2, 1, 1, 0, 0, 2, 5, 2, 1, 3, 
			1, 0, 0, 2, 3, 3, 0, 0, 4, 0, 0, 0, 3, 0, 1, 1, 
			0, 5, 0, 2, 0, 1, 5, 0, 0, 0, 0, 1, 0, 1, 1, 0, 
			5, 1, 0, 0, 1, 3, 4, 5, 5, 0, 2, 0, 1, 4, 3, 1, 
			2, 1, 0, 1, 0, 2, 2, 0, 1, 0, 1, 5, 0, 0, 3, 0, 
			0, 3, 5, 1, 3, 0, 3, 0, 5, 3, 1, 1, 0, 1, 0, 3, 
			5, 1, 0, 2, 0, 1, 3, 0, 0, 0, 5, 3, 2, 0, 5, 5, 
			0, 3, 0, 0, 0, 2, 0, 0, 3, 4, 0, 1, 1, 0, 0, 0, 
			1, 5, 1, 0, 0, 0, 0, 2, 5, 1, 3, 0, 1, 1, 4, 0, 
			2, 3, 0, 2, 2, 0, 0, 0, 0, 5, 1, 0, 3, 0, 0, 0, 
			1, 5, 0, 0, 0, 0, 4, 1, 0, 2, 1, 5, 0, 0, 0, 0, 
			4, 4, 0, 5, 1, 5, 4, 5, 5, 0, 2, 5, 1, 0, 0, 3, 
			0, 0, 1, 3, 0, 4, 0, 0, 2, 0, 0, 2, 0, 0, 0, 1, 
			5, 0, 5, 4, 0, 1, 1, 3, 2, 0, 0, 0, 2, 0, 3, 3
		};

		uint32_t spriteRotations[CELL_SIZE * CELL_SIZE] =
		{
			0, 0, 0, 2, 2, 0, 0, 0, 0, 1, 3, 3, 0, 2, 0, 0,
			0, 1, 3, 3, 0, 2, 0, 0, 0, 0, 0, 0, 0, 2, 3, 1,
			0, 0, 0, 0, 0, 2, 3, 1, 2, 0, 1, 1, 0, 0, 1, 2,
			2, 0, 1, 1, 0, 0, 1, 2, 3, 0, 2, 1, 0, 0, 1, 3,
			3, 0, 2, 1, 0, 0, 1, 3, 2, 0, 0, 1, 2, 3, 1, 0,
			2, 0, 0, 1, 2, 3, 1, 0, 3, 0, 0, 3, 2, 0, 0, 0,
			3, 0, 0, 3, 2, 0, 0, 0, 1, 0, 0, 0, 1, 0, 3, 3,
			1, 0, 0, 0, 1, 0, 3, 3, 0, 1, 0, 0, 0, 3, 1, 0,
			0, 1, 0, 0, 0, 3, 1, 0, 0, 0, 0, 3, 0, 3, 2, 0,
			0, 0, 0, 3, 0, 3, 2, 0, 2, 2, 0, 0, 2, 2, 1, 2,
			2, 2, 0, 0, 2, 2, 1, 2, 3, 0, 0, 0, 0, 3, 2, 0,
			3, 0, 0, 0, 0, 3, 2, 0, 3, 3, 0, 1, 0, 0, 2, 0,
			3, 3, 0, 1, 0, 0, 2, 0, 0, 0, 1, 3, 0, 0, 2, 0,
			0, 0, 1, 3, 0, 0, 2, 0, 0, 0, 3, 0, 3, 0, 3, 0,
			0, 0, 3, 0, 3, 0, 3, 0, 2, 1, 3, 3, 0, 1, 0, 0,
			2, 1, 3, 3, 0, 1, 0, 0, 2, 1, 0, 2, 0, 3, 1, 0
		};

		for (size_t y = 0; y < CELL_SIZE; y++)
		{
			for (size_t x = 0; x < CELL_SIZE; x++)
			{
				Tile tile;
				tile.m_gridLocation = { (float)x, (float)y, 0.0f };
				tile.m_orientation = { 0, 0, 0 };
				tile.m_spriteIndex = spriteIndices[y * CELL_SIZE + x];
				tile.m_rotation = spriteRotations[y * CELL_SIZE + x];
				tile.solid = true;
				cell_0.m_staticTiles[y * CELL_SIZE + x] = tile;
			}
		}

		m_cellGrid.push_back(cell_0);
	}
}

void Scene::onUpdate()
{
	m_player.onUpdate();
}

void Scene::printCellInfo(int cellNumber) 
{
	if (m_cellGrid.empty())
	{
		std::cout << "Scene - printCellInfo: The scene does not contain any cells!" << std::endl;
		return;
	}
	if (cellNumber >= m_cellGrid.size() || cellNumber < 0)
	{
		std::cout << "Scene - printCellInfo: There exists no cell with the number: " << cellNumber << "!" << std::endl;
		return;
	}

	const Cell& cell = m_cellGrid[cellNumber];
	std::cout << "Print cell number: " << cellNumber << "\n";
	// print enemies
	std::cout << "Enemies: { ";
	for (size_t i = 0; i < cell.m_enemies.size(); i++) 
	{
		std::cout << cell.m_enemies[i].m_name;
		if (i != cell.m_enemies.size() - 1)
			std::cout << ",\n\t";
	}
	std::cout << " }\n";
	//print static tiles
	std::cout << "Static tile sprites: { ";
	for (size_t i = 0; i < cell.m_staticTiles.size(); i++)
	{
		if (i == 0)
			std::cout << "\n\t";
		std::cout << cell.m_staticTiles[i].m_spriteIndex;
		if (i == cell.m_staticTiles.size() - 1)
		{
			std::cout << "\n";
			break;
		}
		if ((i + 1) % 16 == 0)
		{
			std::cout << ",\n\t";
			continue;
		}
		std::cout << ", ";
	}
	std::cout << "}\n" << std::endl;	
}
