#include <glm/glm.hpp>

class Camera {
public:
	glm::vec3 m_position{ 0.0f };
	glm::vec3 m_direction{ 0.0f };
	float m_nearPlane = 0.1f, m_farPlane = 100.0f;

	Camera() = default;
	Camera(glm::vec3 position, glm::vec3 direction, float near, float far);

	void setProjection(float fov, float aspect, float near, float far);
	void setView(glm::vec3 direction, glm::vec3 position);
	void setViewTarget(glm::vec3 target, glm::vec3 position);

	const glm::mat4& getProjection() { return m_projection; }
	const glm::mat4& getView() { return m_view; }
private:
	glm::mat4 m_projection{ 1.0f };
	glm::mat4 m_view{ 1.0f };

};