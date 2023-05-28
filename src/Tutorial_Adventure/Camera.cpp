#include "Game.h"
#include "Camera.h"

#include <glm/gtc/matrix_transform.hpp>

void Camera::OnResize()
{
	int width, height;
	glfwGetWindowSize(Game::getInstance().getWindow(), &width, &height);
	if (width == m_width && height == m_height)
		return;
	setProjection((float)width / (float)height);
	m_width = width;
	m_height = height;
}

void Camera::OnUpdate()
{
	const auto& player = Game::getInstance().getActiveScene()->m_player;
	m_position.x = player.m_position.x;
	m_position.y = player.m_position.y - m_cameraHorizontalDistance;
	setViewTarget(player.m_position, m_position);
}

void Camera::setCameraHorizontalDistance(float distance)
{
	m_cameraHorizontalDistance = distance;
}

void Camera::setCameraHeight(float height)
{
	m_position.z = height;
}

void Camera::setProjection(float aspect) 
{
	m_projection = glm::perspective(glm::radians(m_fovRadians), aspect, m_nearPlane, m_farPlane);
}

void Camera::setView(glm::vec3 direction, glm::vec3 position)
{
	m_position = position;
	m_direction = direction;
	m_view = glm::lookAt(position, direction, glm::vec3({ 0.0f, 0.0f, 1.0f }));
}

void Camera::setViewTarget(glm::vec3 target, glm::vec3 position)
{
	m_position = position;
	m_direction = glm::normalize(position - target);
	m_view = glm::lookAt(position, target, glm::vec3({ 0.0f, 0.0f, 1.0f }));
}