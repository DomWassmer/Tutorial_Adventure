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

#include "DescManager.h"
#include "Scene.h"
#include "Vertex.h"

// The static tile sprite sheet is expected top be 160 by 160 pixels containg 10 sprites per row and column
#define STATIC_TILE_SPRITE_SIZE 16
#define STATIC_TILE_TEXTURE_DIMENSION 160
#define STATIC_TILE_TEXTURE_MODULAR 10

// MVP: Model-View-Projection Matrices
struct ModelMatrixPushConstant {
	// Alignment rules don't apply for push constants apparently
	glm::vec3 translate;
	glm::float32_t rotate;
};

struct UniformBufferCameraObject{
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

	struct GraphicsPipelineRessources {
		VkPipelineLayout pipelineLayout;
		VkPipeline graphicsPipeline;
	};

public:
	bool m_framebufferResized = false;
	std::shared_ptr<Scene> m_activeScene;
	VkDevice m_device;
	// With 2 frames in flight the Cpu can always work on the next frame while gpu processes current.
	int MAX_FRAMES_IN_FLIGHT = 2;
	uint32_t m_currentFrame = 0;
public:
	Renderer3D();

	void init();
	void generateSceneRessources();
	void render();
	void cleanup();
	const VkInstance& GetInstance() { return m_instance; }

private:
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
	void createRenderPass();
	void createDescriptorSetLayout();

	void createGraphicsPipelines();
	void createFramebuffers();
	void createCommandPool();
	void createDepthRessources();
	void createTextures();
	void createTextureSampler();
	void createVertexAndIndexBuffers();
	void createUniformBuffers();
	void createCommandBuffers();
	void createDescriptorPool();
	void createDescriptorSets();
	void createSyncObjects();
	void recreateSwapChain();
	void cleanupSwapChain();
	void cleanupSceneRessources();

	// Helper Functions
	std::vector<char> readShaderFromFile(const std::string& filename);
	VkShaderModule createShaderModule(const std::vector<char>& code);
	void createGraphicsPipeline(const std::string& i_vertShaderFilename, const std::string& i_fragShaderFilename,
		const std::vector<VkDescriptorSetLayout>& i_descriptorSetLayouts, VkPushConstantRange* i_pushConstantRange,
		GraphicsPipelineRessources& pipelineRessources);
	void recordCommandBuffer(VkCommandBuffer commandBuffer, uint32_t imageIndex);
	VkCommandBuffer beginSingleTimeCommands();
	void endSingleTimeCommands(VkCommandBuffer commandBuffer);
	uint32_t findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties);
	void createVertexBuffer(VkDeviceSize bufferSize, void* verticesData, VkBuffer& vertexBuffer,
		VkDeviceMemory& vertexBufferMemory);
	void createIndexBuffer(VkDeviceSize bufferSize, void* indexData, VkBuffer& indexBuffer,
		VkDeviceMemory& indexBufferMemory);
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
	void createTextureImage(const char* textureFile, VkImage& textureImage, VkDeviceMemory& textureImageMemory);
	std::array<glm::vec2, 4> queryStaticTileTextureCoords(int index, int rotation);

	// Main Loop
	void drawFrame();
	void updateUniformBuffer(uint32_t currentImage);
	void updatePushConstants(uint32_t currentImage);

private:
	bool m_init = false;
	int m_width = 800;
	int m_height = 600;

	VkInstance m_instance;
	VkSurfaceKHR m_surface;
	VkPhysicalDevice m_physicalDevice;
	VkQueue m_presentQueue;
	VkQueue m_graphicsQueue;
	VkSwapchainKHR m_swapChain;
	std::vector<VkImage> m_swapChainImages;
	VkFormat m_swapChainImageFormat;
	VkExtent2D m_swapChainExtent;

	std::vector<VkImageView> m_swapChainImageViews;
	GraphicsPipelineRessources m_staticPipelineRes;
	GraphicsPipelineRessources m_actorPipelineRes;
	VkRenderPass m_renderPass;
	
	std::vector<VkFramebuffer> m_swapChainFramebuffers;
	VkCommandPool m_commandPool;
	std::vector<VkCommandBuffer> m_commandBuffers;
	VkSampler m_textureSamplerNearest;
	std::vector<VkImage> m_depthImages;
	std::vector<VkDeviceMemory> m_depthImageMemories;
	std::vector<VkImageView> m_depthImageViews;

	SceneRessources m_sceneRessources;
	DescManager m_descriptorManager;

	//Main Loop
	std::vector<VkSemaphore> m_imageAvailableSemaphores; // Semaphores handle order of operations on the gpu
	std::vector<VkSemaphore> m_renderFinishedSemaphores;
	std::vector<VkFence> m_inFlightFences; // Fences handle synchronization to cpu
};