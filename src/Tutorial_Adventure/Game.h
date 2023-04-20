#pragma once

#include <GLFW/glfw3.h>

#include <memory>

#include "Scene.h"
#include "Renderer2D.h"
#include "Renderer3D.h"
#include "Settings.h"

class Game {
public:
	bool m_isRunning = false;
	GLFWwindow* m_window;
	Settings m_settings;

	void init();
	void run();
	void cleanup();

	static Game& getInstance();
private:
	std::unique_ptr<Renderer3D> m_renderer3D;
	std::shared_ptr<Scene> m_activeScene;
};