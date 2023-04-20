#include "Input.h"
#include "Game.h"

bool Input::isMouseButtonDown(MouseButton mouseButton)
{
	int state = glfwGetMouseButton(Game::getInstance().getWindow(), (int)mouseButton);
	return state == GLFW_PRESS;
}

bool Input::isKeyDown(KeyCode keyCode)
{
	int state = glfwGetKey(Game::getInstance().getWindow(), (int)keyCode);
	return state == GLFW_PRESS;
}