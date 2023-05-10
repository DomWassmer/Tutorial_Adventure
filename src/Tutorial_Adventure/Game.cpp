#include <stdexcept>

#include "Game.h"

static Game* g_gameInstance;

void Game::init()
{
	m_isRunning = true;
	g_gameInstance = this;
	if (!glfwInit())
		throw std::runtime_error("GLFW: failed to initialize GLFW!");
	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
	glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);

	m_window = glfwCreateWindow(1280, 960, "Tutorial Adventure", NULL, NULL);
	if (!m_window)
	{
		throw std::runtime_error("GLFW: failed to create window!");
	}

	m_renderer3D = std::make_unique<Renderer3D>(Renderer3D());
	m_renderer3D->init();

	m_activeScene = Scene::generateScene(Scene::SceneType::Level1);
	m_renderer3D->m_activeScene = m_activeScene;
	m_renderer3D->generateSceneRessources();
	m_activeScene->printCellInfo(0);
}

void Game::cleanup()
{
	m_renderer3D->cleanup();

	glfwDestroyWindow(m_window);
	glfwTerminate();

	g_gameInstance = nullptr;
}

void Game::run()
{
	if (glfwWindowShouldClose(m_window))
	{
		m_isRunning = false;
		return;
	}

	glfwPollEvents();
	//glfwGetWindowSize(m_window, &m_width, &m_height);
	m_activeScene->onUpdate();
	m_renderer3D->render();
}

Game& Game::getInstance()
{
	return *g_gameInstance;
}

GLFWwindow* Game::getWindow()
{
	return m_window;
}