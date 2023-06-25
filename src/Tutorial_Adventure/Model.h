#pragma once

#include <vector>

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <vulkan/vulkan.h>

#include "Vertex.h"

class Renderer3D;

class Model3D {
public:
	Model3D(Renderer3D& renderer, const std::vector<Vertex>& vertices, const std::vector<Vertex>& indices);
	~Model3D();

	/* Should not be copied only moved */
	Model3D(const Model3D&) = delete;
	Model3D& operator=(const Model3D&) = delete;

	void bind(VkCommandBuffer commandBuffer);
	void draw(VkCommandBuffer commandBuffer);
private:
	// maybe create abstractions later
	// void createVertexBuffers(const std::vector<Vertex>& vertices);
	// void createIndexBuffers(const std::vector<Vertex>& indices);

	Renderer3D& r_renderer;

	VkBuffer m_vertexBuffer;
	VkDeviceMemory m_vertexBufferMemory;
	VkBuffer m_indexBuffer;
	VkDeviceMemory m_indexBufferMemory;
	uint32_t m_vertexCount;
	uint32_t m_indexCount;

};

/* Optimization for scene object that no matter what never change in a scene */
class StaticModel3D {

};