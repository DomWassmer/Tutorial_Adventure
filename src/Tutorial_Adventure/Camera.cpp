#include "Camera.h"

#include <glm/gtc/matrix_transform.hpp>

Camera::Camera(glm::vec3 position, glm::vec3 direction, float near, float far)
	: m_position(position), m_direction(direction), m_nearPlane(near), m_farPlane(far)
{

}

void Camera::setProjection(float fov, float aspect, float near, float far) 
{
	float m_near = near;
	float m_far = far;
	m_projection = glm::perspective(glm::radians(fov), aspect, near, far);
}

void Camera::setView(glm::vec3 direction, glm::vec3 position)
{
	m_position = position;
	m_direction = direction;
	m_view = glm::lookAt(position, direction, glm::vec3({ 0.0f, 1.0f, 0.0f }));
}

void Camera::setViewTarget(glm::vec3 target, glm::vec3 position)
{
	m_position = position;
	m_direction = glm::normalize(position - target);
	m_view = glm::lookAt(position, m_direction, glm::vec3({ 0.0f, 1.0f, 0.0f }));
}