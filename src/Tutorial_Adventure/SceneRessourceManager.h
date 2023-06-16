#pragma once

#include <vulkan/vulkan.h>

#include <vector>

struct SceneRessources {
	// Global Ressources (camera, ambient light)
	std::vector<VkBuffer> globalUniformBuffers;
	std::vector<VkDeviceMemory> globalUniformBuffersMemory;
	std::vector<void*> globalUniformBuffersMapped;

	// StaticTileRessources
	VkImage staticTileTextureImage;
	VkDeviceMemory staticTileTextureImageMemory;
	VkImageView staticTileTextureImageView;
	VkBuffer staticTileVertexBuffer;
	VkDeviceMemory staticTileVertexBufferMemory;
	VkBuffer staticTileIndexBuffer;
	VkDeviceMemory staticTileIndexBufferMemory;
	std::vector<StaticTileVertex> staticTileVertices;
	std::vector<uint16_t> staticTileIndices;

	// Player Ressources
	VkImage playerTextureImage;
	VkDeviceMemory playerTextureImageMemory;
	VkImageView playerTextureImageView;
	VkBuffer playerVertexBuffer;
	VkDeviceMemory playerVertexBufferMemory;
	std::vector<VkBuffer> playerIndexBuffers;
	std::vector<VkDeviceMemory> playerIndexBufferMemories;
	std::vector<Vertex> playerVertices;
	std::vector<uint16_t> playerIndices;

	// Enemy Ressources
	std::vector<VkImage> EnemyTextureImages;
	std::vector<VkDeviceMemory> playerTextureImageMemory;
	VkImageView playerTextureImageView;
	VkBuffer playerVertexBuffer;
	VkDeviceMemory playerVertexBufferMemory;
	std::vector<VkBuffer> playerIndexBuffers;
	std::vector<VkDeviceMemory> playerIndexBufferMemories;
	std::vector<Vertex> playerVertices;
	std::vector<uint16_t> playerIndices;

};

class SceneRessourceManager {

};