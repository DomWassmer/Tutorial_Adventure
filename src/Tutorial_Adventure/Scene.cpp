#include "Scene.h"

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
		Cell cell_0;
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

		for (size_t z = 0; z < CELL_SIZE; z++)
		{
			for (size_t x = 0; x < CELL_SIZE; x++)
			{
				Tile tile;
				int gridLocation[3] = { x, 0, z };
				memcpy(&tile.m_gridLocation, &gridLocation, 3 * sizeof(int));
				int orientation[3] = { 0, 0, 0 };
				memcpy(&tile.m_orientation, &orientation, 3 * sizeof(int));
				tile.m_spriteIndex = spriteIndices[z * CELL_SIZE + x];
				tile.m_rotation = spriteRotations[z * CELL_SIZE + x];
				tile.solid = true;
				cell_0.m_staticTiles[z * CELL_SIZE + x] = tile;
			}
		}

		m_cellGrid.push_back(cell_0);
	}
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