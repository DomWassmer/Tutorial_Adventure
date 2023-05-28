#pragma once

#include <glm/glm.hpp>

class Camera {
public:
	Camera() = default;
	
	void OnResize();
	void OnUpdate();

	void setCameraHorizontalDistance(float distance);
	void setCameraHeight(float height);
	void setProjection(float aspect);
	void setView(glm::vec3 direction, glm::vec3 position);
	void setViewTarget(glm::vec3 target, glm::vec3 position);

	const glm::mat4& getProjection() { return m_projection; }
	const glm::mat4& getView() { return m_view; }
private:
	glm::vec3 m_position{ 0.0f, 0.0f, 6.0f };
	glm::vec3 m_direction{ 0.0f };
	float m_cameraHorizontalDistance = 0.0f;
	float m_nearPlane = 0.1f, m_farPlane = 100.0f;
	float m_fovRadians = 45.0f;

	float m_width = 0.0f, m_height = 0.0f;
	glm::mat4 m_projection{ 1.0f };
	glm::mat4 m_view{ 1.0f };

};