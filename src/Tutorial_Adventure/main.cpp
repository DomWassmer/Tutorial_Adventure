#include <iostream>

#include "Game.h"

int main(int argc, char* argv[]) {
	Game game;

	try {
		game.init();

		while (game.m_isRunning)
		{
			game.run();
		}

		game.cleanup();
	} 
	catch (const std::exception& e)
	{
		// Handle exception logging here!
		std::cout << e.what() << std::endl;
	}
}