#include "Model.h"

#include "Renderer3D.h"

Model3D::Model3D(Renderer3D& renderer,const std::vector<Vertex>& vertices,const std::vector<Vertex>& indices) 
	: r_renderer(renderer)
{
	m_vertexCount = static_cast<uint32_t>(vertices.size());
	VkDeviceSize bufferSize = sizeof(Vertex) * m_vertexCount;
	r_renderer.createVertexBuffer(bufferSize, vertices.data(), m_vertexBuffer, m_vertexBufferMemory);
	m_indexCount = static_cast<uint32_t>(indices.size());
	bufferSize = sizeof(uint16_t) * m_indexCount;
	r_renderer.createIndexBuffer(bufferSize, indices.data(), m_indexBuffer, m_indexBufferMemory);
}

Model3D::~Model3D()
{
	vkDestroyBuffer(r_renderer.m_device, m_vertexBuffer, nullptr);
	vkFreeMemory(r_renderer.m_device, m_vertexBufferMemory, nullptr);
	vkDestroyBuffer(r_renderer.m_device, m_indexBuffer, nullptr);
	vkFreeMemory(r_renderer.m_device, m_indexBufferMemory, nullptr);
}

void Model3D::bind(VkCommandBuffer commandBuffer)
{
	VkBuffer vertexBuffers[] = { m_vertexBuffer };
	VkDeviceSize offsets[] = { 0 };
	vkCmdBindVertexBuffers(commandBuffer, 0, 1, vertexBuffers, offsets);
	vkCmdBindIndexBuffer(commandBuffer, m_indexBuffer, 0, VK_INDEX_TYPE_UINT16);
}

void Model3D::draw(VkCommandBuffer commandBuffer)
{
	vkCmdDrawIndexed(commandBuffer, m_indexCount, 1, 0, 0, 0)
}