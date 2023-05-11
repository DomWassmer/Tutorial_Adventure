#include <vulkan/vulkan.h>

#include <glm/glm.hpp>

struct Vertex {
	glm::vec3 pos;
	glm::vec2 texCoord;

	static VkVertexInputBindingDescription getBindingDescription();
	static std::array<VkVertexInputAttributeDescription, 2> getAttributeDescriptions();
};

struct StaticTileVertex {
	glm::vec3 worldPos;
	glm::vec2 texCoord;

	static VkVertexInputBindingDescription getBindingDescription();
	static std::array<VkVertexInputAttributeDescription, 2> getAttributeDescriptions();
};
