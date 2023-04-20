#pragma once

#include <optional>
#include <vector>
#include <string>
#include <array>
#include <chrono>

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <vulkan/vulkan.h>

#include "Scene.h"

struct Vertex {
	glm::vec3 pos;
	glm::vec3 color;
	glm::vec2 texCoord;

	static VkVertexInputBindingDescription getBindingDescription();
	static std::array<VkVertexInputAttributeDescription, 3> getAttributeDescriptions();
};

// MVP: Model-View-Projection Matrices
struct UniformBufferObject {
	alignas(16) glm::mat4 model;
	alignas(16) glm::mat4 view;
	alignas(16) glm::mat4 proj;
};

class Renderer3D {
public:
	struct QueueFamilyIndices {
		// optional because 0 is valid
		std::optional<uint32_t> graphicsFamily;
		std::optional<uint32_t> presentFamily;

		bool is_complete()
		{
			return graphicsFamily.has_value()
				&& presentFamily.has_value();
		}
	};

	struct SwapChainSupportDetails {
		VkSurfaceCapabilitiesKHR capabilities;
		std::vector<VkSurfaceFormatKHR> formats;
		std::vector<VkPresentModeKHR> presentModes;
	};

public:
	bool m_framebufferResized = false;

public:
	void run();
	const VkInstance& GetInstance() { return m_instance; }

private:
	void initWindow();
	void initVulkan();
	void mainLoop();
	void cleanup();

	// initVulkan functions
	void createInstance();
	//void setupDebugMessenger();
	void createSurface();
	void pickPhysicalDevice();
	bool isDeviceSuitable(VkPhysicalDevice device);
	bool checkDeviceExtensionSupport(VkPhysicalDevice device);
	QueueFamilyIndices findQueueFamilies(VkPhysicalDevice device);
	SwapChainSupportDetails querySwapChainSupport(VkPhysicalDevice device);
	void createLogicalDevice();
	VkSurfaceFormatKHR chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats);
	VkPresentModeKHR chooseSwapPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes);
	VkExtent2D chooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities);
	void createSwapChain();
	void createImageViews();

	// Probably inside Renderer
	void createRenderPass();
	void createDescriptorSetLayout();
	void createGraphicsPipeline();
	void createFramebuffers();
	void createCommandPool();
	void createDepthRessources();
	void createTextureImage();
	void createTextureImageView();
	void createTextureSampler();
	void createVertexBuffer();
	void createIndexBuffer();
	void createUniformBuffers();
	void createCommandBuffers();
	void createDescriptorPool();
	void createDescriptorSets();
	void createSyncObjects();
	void recreateSwapChain();
	void cleanupSwapChain();

	// Helper Functions
	std::vector<char> readShaderFromFile(const std::string& filename);
	VkShaderModule createShaderModule(const std::vector<char>& code);
	void recordCommandBuffer(VkCommandBuffer commandBuffer, uint32_t imageIndex);
	VkCommandBuffer beginSingleTimeCommands();
	void endSingleTimeCommands(VkCommandBuffer commandBuffer);
	uint32_t findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties);
	void createBuffer(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties,
		VkBuffer& buffer, VkDeviceMemory& bufferMemory);
	void copyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size);
	void createImage(uint32_t width, uint32_t height, VkFormat format, VkImageTiling tiling,
		VkImageUsageFlags usage, VkMemoryPropertyFlags properties, VkImage& image, VkDeviceMemory& imageMemory);
	void transitionImageLayout(VkImage image, VkFormat format, VkImageLayout oldLayout, VkImageLayout newLayout);
	void copyBufferToImage(VkBuffer buffer, VkImage image, uint32_t width, uint32_t height);
	VkImageView createImageView(VkImage image, VkFormat format, VkImageAspectFlags aspectFlags);
	VkFormat findSupportedFormat(const std::vector<VkFormat>& candidates, VkImageTiling tiling, VkFormatFeatureFlags features);
	VkFormat findDepthFormat();
	bool hasStencilComponent(VkFormat format);

	// Main Loop
	void drawFrame();
	void updateUniformBuffer(uint32_t currentImage);

private:
	// With 2 frames in flight the Cpu can always work on the next frame while gpu processes current.
	const int MAX_FRAMES_IN_FLIGHT = 2;
	uint32_t m_currentFrame = 0;

	int m_width = 800;
	int m_height = 600;
	GLFWwindow* m_window;

	VkInstance m_instance;
	VkSurfaceKHR m_surface;
	VkPhysicalDevice m_physicalDevice;
	VkQueue m_presentQueue;
	VkDevice m_device;
	VkQueue m_graphicsQueue;
	VkSwapchainKHR m_swapChain;
	std::vector<VkImage> m_swapChainImages;
	VkFormat m_swapChainImageFormat;
	VkExtent2D m_swapChainExtent;

	// Probably extracted into own image class of a renderer
	std::vector<VkImageView> m_swapChainImageViews;
	VkDescriptorSetLayout m_descriptorSetLayout;
	VkPipelineLayout m_pipelineLayout;
	VkRenderPass m_renderPass;
	VkPipeline m_graphicsPipeline;
	std::vector<VkFramebuffer> m_swapChainFramebuffers;
	VkCommandPool m_commandPool;
	std::vector<VkCommandBuffer> m_commandBuffers;
	VkImage m_textureImage;
	VkDeviceMemory m_textureImageMemory;
	VkImageView m_textureImageView;
	VkSampler m_textureSampler;
	VkBuffer m_vertexBuffer;
	VkDeviceMemory m_vertexBufferMemory;
	VkBuffer m_indexBuffer;
	VkDeviceMemory m_indexBufferMemory;
	std::vector<VkBuffer> m_uniformBuffers;
	std::vector<VkDeviceMemory> m_uniformBuffersMemory;
	std::vector<void*> m_uniformBuffersMapped;
	VkDescriptorPool m_descriptorPool;
	std::vector<VkDescriptorSet> m_descriptorSets;
	std::vector<VkImage> m_depthImages;
	std::vector<VkDeviceMemory> m_depthImageMemories;
	std::vector<VkImageView> m_depthImageViews;

	std::vector<Vertex> m_vertices = {
		{{-0.5f, -0.5f, 0.0f}, {1.0f, 0.0f, 0.0f}, {1.0f, 0.0f}},
		{{ 0.5f, -0.5f, 0.0f}, {0.0f, 1.0f, 0.0f}, {0.0f, 0.0f}},
		{{ 0.5f,  0.5f, 0.0f}, {0.0f, 0.0f, 1.0f}, {0.0f, 1.0f}},
		{{-0.5f,  0.5f, 0.0f}, {1.0f, 1.0f, 1.0f}, {1.0f, 1.0f}},

		{{-0.5f, -0.5f, -0.5f}, {1.0f, 0.0f, 0.0f}, {1.0f, 0.0f}},
		{{ 0.5f, -0.5f, -0.5f}, {0.0f, 1.0f, 0.0f}, {0.0f, 0.0f}},
		{{ 0.5f,  0.5f, -0.5f}, {0.0f, 0.0f, 1.0f}, {0.0f, 1.0f}},
		{{-0.5f,  0.5f, -0.5f}, {1.0f, 1.0f, 1.0f}, {1.0f, 1.0f}}
	};

	std::vector<uint16_t> m_indices = {
		0, 1, 2, 2, 3, 0,
		4, 5, 6, 6, 7, 4
	};

	//Main Loop
	std::vector<VkSemaphore> m_imageAvailableSemaphores; // Semaphores handle order of operations on the gpu
	std::vector<VkSemaphore> m_renderFinishedSemaphores;
	std::vector<VkFence> m_inFlightFences; // Fences handle synchronization to cpu
};