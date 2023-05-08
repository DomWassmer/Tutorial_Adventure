#include "Renderer3D.h"
#include "Game.h"

#include <stdexcept>
#include <iostream>
#include <set>
#include <limits>
#include <algorithm>
#include <fstream>

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#ifdef _DEBUG
#define USE_VK_VALIDATION_LAYERS
#ifdef USE_VK_VALIDATION_LAYERS
const std::vector<const char*> g_validationLayers = { "VK_LAYER_KHRONOS_validation" };
#endif // USE_VK_VALIDATION_LAYERS
#endif // DEBUG
std::vector<const char*> g_deviceExtensions = { VK_KHR_SWAPCHAIN_EXTENSION_NAME, VK_KHR_MAINTENANCE1_EXTENSION_NAME };

#define ASSET_PATH "../../../assets/"
#define SHADER_PATH "../../../shaders/"

// Can't be a member function because compiler changes member function to non-member function func(this, args)
static void framebufferResizeCallback(GLFWwindow* window, int width, int height)
{
	auto app = reinterpret_cast<Renderer3D*>(glfwGetWindowUserPointer(window));
	app->m_framebufferResized = true;
}

void Renderer3D::init()
{
	if (m_init)
		return;
	glfwSetWindowUserPointer(Game::getInstance().getWindow(), this);
	glfwSetFramebufferSizeCallback(Game::getInstance().getWindow(), framebufferResizeCallback);
	createInstance();
	//setupDebugMessenger();
	createSurface();
	pickPhysicalDevice();
	createLogicalDevice();
}

void Renderer3D::generateSceneRessources()
{
	createSwapChain();
	createImageViews();
	createRenderPass();
	createDescriptorSetLayout();
	createGraphicsPipelines();
	createCommandPool();
	createDepthRessources();
	createFramebuffers();
	createTextureSampler();
	createUniformBuffers();
	createCommandBuffers();
	createSyncObjects();
	
	// Load all texture ressources for current scene
	createTextures();

	// Load all vertex and buffer ressources for current scene
	createVertexAndIndexBuffers();

	// Load all descriptor ressources for current scene
	createDescriptorPool();
	createDescriptorSets();
}

void Renderer3D::render()
{
	glfwGetWindowSize(Game::getInstance().getWindow(), &m_width, &m_height);
	m_activeScene->m_activeCamera.OnResize();
	m_activeScene->m_activeCamera.OnUpdate();
	drawFrame();
	// or: vkQueueWaitIdle(m_device);
	vkDeviceWaitIdle(m_device);
}

void Renderer3D::cleanup()
{
	// Order important for some of the operations
	cleanupSwapChain();

	cleanupSceneRessources();

	vkDestroyRenderPass(m_device, m_renderPass, nullptr);

	for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
	{
		vkDestroySemaphore(m_device, m_imageAvailableSemaphores[i], nullptr);
		vkDestroySemaphore(m_device, m_renderFinishedSemaphores[i], nullptr);
		vkDestroyFence(m_device, m_inFlightFences[i], nullptr);
	}

	vkDestroyCommandPool(m_device, m_commandPool, nullptr);

	vkDestroyDevice(m_device, nullptr);

	vkDestroySurfaceKHR(m_instance, m_surface, nullptr);
	vkDestroyInstance(m_instance, nullptr);
}

void Renderer3D::createInstance()
{
#ifdef USE_VK_VALIDATION_LAYERS
	uint32_t layerCount;
	vkEnumerateInstanceLayerProperties(&layerCount, nullptr);
	std::vector<VkLayerProperties> availableLayers(layerCount);
	vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data());

	for (const char* layerName : g_validationLayers)
	{
		bool layerFound = false;
		for (const auto& layerProperties : availableLayers)
		{
			if (!strcmp(layerName, layerProperties.layerName))
			{
				layerFound = true;
				break;
			}
		}
		if (!layerFound)
		{
			throw std::runtime_error("validation layer requested, but not available!");
		}
		std::cout << "Found validation layer: " << layerName << "\n";
	}
#endif

	VkApplicationInfo appInfo{};
	appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
	appInfo.pApplicationName = "Vulkan from scratch";
	appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
	appInfo.pEngineName = "No Engine";
	appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
	appInfo.apiVersion = VK_MAKE_VERSION(1, 0, 0);

	VkInstanceCreateInfo createInfo{};
	createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
	createInfo.pApplicationInfo = &appInfo;
	uint32_t glfwExtensionCount = 0;
	const char** glfwExtensions;
	glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);
	createInfo.enabledExtensionCount = glfwExtensionCount;
	createInfo.ppEnabledExtensionNames = glfwExtensions;
	createInfo.enabledLayerCount = 0;
#ifdef USE_VK_VALIDATION_LAYERS
	createInfo.enabledLayerCount = (uint32_t)g_validationLayers.size();
	createInfo.ppEnabledLayerNames = g_validationLayers.data();
#endif

	// Check if certain extensions are present
	uint32_t extensionCount = 0;
	vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, nullptr);
	std::vector<VkExtensionProperties> extensions(extensionCount);
	vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, extensions.data());
	for (const auto& extension : extensions)
	{
		// Check if extension is present
	}

	if (vkCreateInstance(&createInfo, nullptr, &m_instance) != VK_SUCCESS)
	{
		throw std::runtime_error("Vulkan: failed to create instance!");
	}
}

void Renderer3D::createSurface()
{
	if (glfwCreateWindowSurface(m_instance, Game::getInstance().getWindow(), nullptr, &m_surface) != VK_SUCCESS)
		throw std::runtime_error("Vulkan: failed to create window surface!");
}

void Renderer3D::pickPhysicalDevice()
{
	uint32_t deviceCount = 0;
	vkEnumeratePhysicalDevices(m_instance, &deviceCount, nullptr);
	if (!deviceCount)
		throw std::runtime_error("failed to find GPUs with Vulkan support!");
	std::vector<VkPhysicalDevice> devices(deviceCount);
	vkEnumeratePhysicalDevices(m_instance, &deviceCount, devices.data());

	for (const auto& device : devices)
	{
		if (isDeviceSuitable(device))
		{
			// just takes the first suitable device -> good enough for this purpose
			m_physicalDevice = device;
			break;
		}
	}
	if (!m_physicalDevice)
		throw std::runtime_error("failed to find a suitable GPU");


#ifdef VERBOSE
	VkPhysicalDeviceProperties deviceProperties;
	vkGetPhysicalDeviceProperties(m_physicalDevice, &deviceProperties);
	std::cout << "Vulkan: Selected physical device: " << deviceProperties.deviceName << "\n";
#endif // VERBOSE

}

bool Renderer3D::isDeviceSuitable(VkPhysicalDevice device)
{
	VkPhysicalDeviceProperties deviceProperties;
	VkPhysicalDeviceFeatures deviceFeatures;
	vkGetPhysicalDeviceProperties(device, &deviceProperties);
	vkGetPhysicalDeviceFeatures(device, &deviceFeatures);
	if (!(deviceProperties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU && deviceFeatures.geometryShader))
	{
		std::cout << "Device is missing VkPhysicalDeviceProperties or Features!" << std::endl;
		return false;
	}

	if (!findQueueFamilies(device).is_complete())
	{
		std::cout << "Device is missing QueueFamilies!" << std::endl;
		return false;
	}

	if (!checkDeviceExtensionSupport(device))
	{
		std::cout << "Device is missing Extension Support!" << std::endl;
		return false;
	}

	if (!deviceFeatures.samplerAnisotropy)
	{
		std::cout << "Device is missing the sampler anisotropy feature!" << std::endl;
		return false;
	}

	SwapChainSupportDetails SCSdetails = querySwapChainSupport(device);
	if (SCSdetails.formats.empty() || SCSdetails.presentModes.empty())
	{
		std::cout << "Device is missing required Swap Chain Support!" << std::endl;
		return false;
	}
	return true;
}

bool Renderer3D::checkDeviceExtensionSupport(VkPhysicalDevice device)
{
	uint32_t extensionCount;
	vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, nullptr);
	std::vector<VkExtensionProperties> availableExtensions(extensionCount);
	vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, availableExtensions.data());
	std::set<std::string> requiredExtensions(g_deviceExtensions.begin(), g_deviceExtensions.end());

	for (const auto& extension : availableExtensions)
	{
		requiredExtensions.erase(extension.extensionName);
	}
	return requiredExtensions.empty();
}

Renderer3D::QueueFamilyIndices Renderer3D::findQueueFamilies(VkPhysicalDevice device)
{
	QueueFamilyIndices indices;
	uint32_t queueFamilyCount = 0;
	vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, nullptr);
	std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
	vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, queueFamilies.data());
	for (size_t i = 0; i < queueFamilyCount; i++)
	{
		// Why bitwise and?
		if (queueFamilies[i].queueFlags & VK_QUEUE_GRAPHICS_BIT)
			indices.graphicsFamily = i;
		VkBool32 presentSupport = false;
		vkGetPhysicalDeviceSurfaceSupportKHR(device, i, m_surface, &presentSupport);
		if (presentSupport)
			indices.presentFamily = i;
	}


	return indices;
}

Renderer3D::SwapChainSupportDetails Renderer3D::querySwapChainSupport(VkPhysicalDevice device)
{
	SwapChainSupportDetails details;
	vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device, m_surface, &details.capabilities);

	uint32_t formatCount;
	vkGetPhysicalDeviceSurfaceFormatsKHR(device, m_surface, &formatCount, nullptr);
	if (formatCount)
	{
		details.formats.resize(formatCount);
		vkGetPhysicalDeviceSurfaceFormatsKHR(device, m_surface, &formatCount, details.formats.data());
	}

	uint32_t presentModeCount;
	vkGetPhysicalDeviceSurfacePresentModesKHR(device, m_surface, &presentModeCount, nullptr);
	if (presentModeCount)
	{
		details.presentModes.resize(presentModeCount);
		vkGetPhysicalDeviceSurfacePresentModesKHR(device, m_surface, &presentModeCount, details.presentModes.data());
	}

	return details;
}

void Renderer3D::createLogicalDevice()
{
	QueueFamilyIndices indices = findQueueFamilies(m_physicalDevice);

	std::vector< VkDeviceQueueCreateInfo> queueCreateInfos;
	std::set<uint32_t> uniqueQueueFamilies = { indices.graphicsFamily.value(), indices.presentFamily.value() };

	float queuePriority = 1.0f;
	for (uint32_t queueFamily : uniqueQueueFamilies)
	{
		VkDeviceQueueCreateInfo queueCreateInfo{};
		queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
		queueCreateInfo.queueFamilyIndex = queueFamily;
		queueCreateInfo.queueCount = 1;
		// has to present even for one, but if more queues are there this assigns the command schedule order 
		// of the command buffers of the queue [0.0f, 1.0f]
		queueCreateInfo.pQueuePriorities = &queuePriority;
		queueCreateInfos.push_back(queueCreateInfo);
	}

	// For now irrelevant becomes relevant for raytracing for example
	VkPhysicalDeviceFeatures deviceFeatures{};
	deviceFeatures.samplerAnisotropy = VK_TRUE;

	VkDeviceCreateInfo createInfo{};
	createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
	createInfo.pQueueCreateInfos = queueCreateInfos.data();
	createInfo.queueCreateInfoCount = (uint32_t)queueCreateInfos.size();
	createInfo.pEnabledFeatures = &deviceFeatures;
	createInfo.enabledExtensionCount = (uint32_t)g_deviceExtensions.size();
	createInfo.ppEnabledExtensionNames = g_deviceExtensions.data();
#ifdef USE_VK_VALIDATION_LAYERS
	createInfo.enabledLayerCount = (uint32_t)g_validationLayers.size();
	createInfo.ppEnabledLayerNames = g_validationLayers.data();
#endif
	if (vkCreateDevice(m_physicalDevice, &createInfo, nullptr, &m_device) != VK_SUCCESS)
		throw std::runtime_error("Vulkan: failed to create logical device!");
	vkGetDeviceQueue(m_device, indices.graphicsFamily.value(), 0, &m_graphicsQueue);
	vkGetDeviceQueue(m_device, indices.presentFamily.value(), 0, &m_presentQueue);
}

VkSurfaceFormatKHR Renderer3D::chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats)
{
	// Just pick the first and be happy
	for (const auto& availableFormat : availableFormats)
	{
		if (availableFormat.format == VK_FORMAT_B8G8R8A8_SRGB &&
			availableFormat.colorSpace == VK_COLORSPACE_SRGB_NONLINEAR_KHR)
			return availableFormat;
	}
	return availableFormats[0];
}

VkPresentModeKHR Renderer3D::chooseSwapPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes)
{
	// VK_PRESENT_MODE_FIFO_KHR is guaranteed to be there
	// if energy usage is no concern author recommends VK_PRESENT_MODE_MAILBOX_KHR
	for (const auto& availablePresentMode : availablePresentModes)
	{
		if (availablePresentMode == VK_PRESENT_MODE_MAILBOX_KHR)
			return availablePresentMode;
	}

	return VK_PRESENT_MODE_FIFO_KHR;
}

VkExtent2D Renderer3D::chooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities)
{
	if (capabilities.currentExtent.width != std::numeric_limits<uint32_t>::max())
	{
		return capabilities.currentExtent;
	}
	else
	{
		int width, height;
		glfwGetFramebufferSize(Game::getInstance().getWindow(), &width, &height);

		VkExtent2D actualExtent = { (uint32_t)width, (uint32_t)height };
		actualExtent.width = std::clamp(actualExtent.width, capabilities.minImageExtent.width, capabilities.maxImageExtent.width);
		actualExtent.height = std::clamp(actualExtent.height, capabilities.minImageExtent.height, capabilities.maxImageExtent.height);
		return actualExtent;
	}
}

void Renderer3D::createSwapChain()
{
	SwapChainSupportDetails swapChainSupport = querySwapChainSupport(m_physicalDevice);
	VkSurfaceFormatKHR surfaceFormat = chooseSwapSurfaceFormat(swapChainSupport.formats);
	VkPresentModeKHR presentMode = chooseSwapPresentMode(swapChainSupport.presentModes);
	VkExtent2D extent = chooseSwapExtent(swapChainSupport.capabilities);
	// Recommended to have at least one more image space in swap chain as the minimum.
	uint32_t imageCount = swapChainSupport.capabilities.minImageCount + 1;
	if (swapChainSupport.capabilities.maxImageCount > 0 && imageCount > swapChainSupport.capabilities.maxImageCount)
		imageCount = swapChainSupport.capabilities.maxImageCount;
	MAX_FRAMES_IN_FLIGHT = imageCount;

	VkSwapchainCreateInfoKHR createInfo{};
	createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
	createInfo.surface = m_surface;
	createInfo.minImageCount = imageCount;
	createInfo.imageFormat = surfaceFormat.format;
	createInfo.imageColorSpace = surfaceFormat.colorSpace;
	createInfo.imageExtent = extent;
	createInfo.imageArrayLayers = 1;
	// Use VK_IMAGE_USAGE_TRANSFER_DST_BIT for postprocessing in memory
	createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
	QueueFamilyIndices indices = findQueueFamilies(m_physicalDevice);
	uint32_t queueFamilyIndices[] = { indices.graphicsFamily.value(), indices.presentFamily.value() };
	if (indices.graphicsFamily != indices.presentFamily)
	{
		createInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
		createInfo.queueFamilyIndexCount = 2;
		createInfo.pQueueFamilyIndices = queueFamilyIndices;
	}
	else
	{
		createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
		// Others null
	}
	createInfo.preTransform = swapChainSupport.capabilities.currentTransform;
	createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
	createInfo.presentMode = presentMode;
	createInfo.clipped = VK_TRUE;
	createInfo.oldSwapchain = VK_NULL_HANDLE;

	if (vkCreateSwapchainKHR(m_device, &createInfo, nullptr, &m_swapChain) != VK_SUCCESS)
	{
		throw std::runtime_error("Vulkan: failed to create swap chain");
	}

	vkGetSwapchainImagesKHR(m_device, m_swapChain, &imageCount, nullptr);
	m_swapChainImages.resize(imageCount);
	vkGetSwapchainImagesKHR(m_device, m_swapChain, &imageCount, m_swapChainImages.data());
	m_swapChainImageFormat = surfaceFormat.format;
	m_swapChainExtent = extent;
}

void Renderer3D::createImageViews()
{
	m_swapChainImageViews.resize(m_swapChainImages.size());
	for (size_t i = 0; i < m_swapChainImages.size(); i++)
	{
		m_swapChainImageViews[i] = createImageView(m_swapChainImages[i], m_swapChainImageFormat, VK_IMAGE_ASPECT_COLOR_BIT);
	}
}

void Renderer3D::createRenderPass()
{
	VkAttachmentDescription colorAttachment{};
	colorAttachment.format = m_swapChainImageFormat;
	colorAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
	colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
	colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
	colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE; // Currently doesn't use stencil buffer
	colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
	colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	colorAttachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

	VkAttachmentReference colorAttachmentRef{};
	colorAttachmentRef.attachment = 0;
	colorAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

	VkSubpassDependency dependency{};
	dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
	dependency.dstSubpass = 0;
	dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT
		| VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
	dependency.srcAccessMask = 0;
	dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT
		| VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
	dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT
		| VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;

	VkAttachmentDescription depthAttachment{};
	depthAttachment.format = findDepthFormat();
	depthAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
	depthAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
	depthAttachment.storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
	depthAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
	depthAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
	depthAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	depthAttachment.finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

	VkAttachmentReference depthAttachmentRef{};
	depthAttachmentRef.attachment = 1;
	depthAttachmentRef.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

	VkSubpassDescription subpass{};
	subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
	subpass.colorAttachmentCount = 1;
	subpass.pColorAttachments = &colorAttachmentRef;
	subpass.pDepthStencilAttachment = &depthAttachmentRef;

	std::array<VkAttachmentDescription, 2> attachments = { colorAttachment, depthAttachment };
	VkRenderPassCreateInfo renderPassInfo{};
	renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
	renderPassInfo.attachmentCount = (uint32_t)attachments.size();
	renderPassInfo.pAttachments = attachments.data();
	renderPassInfo.dependencyCount = 1;
	renderPassInfo.pDependencies = &dependency;
	renderPassInfo.subpassCount = 1;
	renderPassInfo.pSubpasses = &subpass;
	if (vkCreateRenderPass(m_device, &renderPassInfo, nullptr, &m_renderPass) != VK_SUCCESS)
		throw std::runtime_error("failed to create render pass!");
}

void Renderer3D::createDescriptorSetLayout()
{
	// Global Set Layout
	{
		VkDescriptorSetLayoutBinding uboLayoutBinding{};
		uboLayoutBinding.binding = 0;
		uboLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
		uboLayoutBinding.descriptorCount = 1;
		uboLayoutBinding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
		uboLayoutBinding.pImmutableSamplers = nullptr; // Optional

		VkDescriptorSetLayoutCreateInfo layoutInfo{};
		layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
		layoutInfo.bindingCount = 1;
		layoutInfo.pBindings = &uboLayoutBinding;

		if (vkCreateDescriptorSetLayout(m_device, &layoutInfo, nullptr, &m_sceneRessources.globalDescriptorSetLayout) != VK_SUCCESS)
			throw std::runtime_error("Vulkan: failed to create descriptor set layout!");
	}

	// Static Tile Set Layout
	{
		VkDescriptorSetLayoutBinding samplerLayoutBinding{};
		samplerLayoutBinding.binding = 0;
		samplerLayoutBinding.descriptorCount = 1;
		samplerLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
		samplerLayoutBinding.pImmutableSamplers = nullptr;
		samplerLayoutBinding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;

		VkDescriptorSetLayoutCreateInfo layoutInfo{};
		layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
		layoutInfo.bindingCount = 1;
		layoutInfo.pBindings = &samplerLayoutBinding;
		
		if (vkCreateDescriptorSetLayout(m_device, &layoutInfo, nullptr, &m_sceneRessources.staticTileDescriptorSetLayout) != VK_SUCCESS)
			throw std::runtime_error("Vulkan: failed to create descriptor set layout!");
	}
}

void Renderer3D::createGraphicsPipelines()
{
	{
		std::string vertShader = SHADER_PATH "StaticTileVert.spv";
		std::string frageShader = SHADER_PATH "StaticTileFrag.spv";
		createGraphicsPipeline(vertShader, frageShader, nullptr, m_staticPipelineRes);
	}

	{
		std::string vertShader = SHADER_PATH "playerVert.spv";
		std::string frageShader = SHADER_PATH "playerFrag.spv";

		VkPushConstantRange pushConstantRange{};
		// For now I only want to access it in the fragment shader. Maybe later use it in frag shader for ambient light to.
		pushConstantRange.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
		pushConstantRange.offset = 0;
		pushConstantRange.size = sizeof(ModelMatrixPushConstant);
		createGraphicsPipeline(vertShader, frageShader, &pushConstantRange, m_actorPipelineRes);
	}
}

void Renderer3D::createFramebuffers()
{
	m_swapChainFramebuffers.resize(m_swapChainImageViews.size());
	for (size_t i = 0; i < m_swapChainImageViews.size(); i++)
	{
		std::array<VkImageView, 2> attachments = { m_swapChainImageViews[i], m_depthImageViews[i] };

		VkFramebufferCreateInfo framebufferInfo{};
		framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
		framebufferInfo.renderPass = m_renderPass;
		framebufferInfo.attachmentCount = (uint32_t)attachments.size();
		framebufferInfo.pAttachments = attachments.data();
		framebufferInfo.width = m_swapChainExtent.width;
		framebufferInfo.height = m_swapChainExtent.height;
		framebufferInfo.layers = 1;

		if (vkCreateFramebuffer(m_device, &framebufferInfo, nullptr, &m_swapChainFramebuffers[i]) != VK_SUCCESS)
			throw std::runtime_error("VK: failed to create framebuffer!");
	}
}

void Renderer3D::createCommandPool()
{
	QueueFamilyIndices queueFamiliyIndices = findQueueFamilies(m_physicalDevice);
	VkCommandPoolCreateInfo poolInfo{};
	poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
	poolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
	poolInfo.queueFamilyIndex = queueFamiliyIndices.graphicsFamily.value();
	if (vkCreateCommandPool(m_device, &poolInfo, nullptr, &m_commandPool))
		throw std::runtime_error("VK: failed to create command pool!");
}

void Renderer3D::createDepthRessources()
{
	VkFormat depthFormat = findDepthFormat();
	m_depthImages.resize(MAX_FRAMES_IN_FLIGHT);
	m_depthImageMemories.resize(MAX_FRAMES_IN_FLIGHT);
	m_depthImageViews.resize(MAX_FRAMES_IN_FLIGHT);
	for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
	{
		createImage(m_swapChainExtent.width, m_swapChainExtent.height,
			depthFormat, VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT,
			VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, m_depthImages[i], m_depthImageMemories[i]);
		m_depthImageViews[i] = createImageView(m_depthImages[i], depthFormat, VK_IMAGE_ASPECT_DEPTH_BIT);
		transitionImageLayout(m_depthImages[i], depthFormat, VK_IMAGE_LAYOUT_UNDEFINED,
			VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL);
	}
}

void Renderer3D::createTextures()
{
	{
		std::string FloorTextureFile = ASSET_PATH "Sprite Floor Tiles.png";
		createTextureImage(FloorTextureFile.c_str(), m_sceneRessources.staticTileTextureImage, 
			m_sceneRessources.staticTileTextureImageMemory);
		m_sceneRessources.staticTileTextureImageView = createImageView(m_sceneRessources.staticTileTextureImage, 
			VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_ASPECT_COLOR_BIT);
	}

	{
		std::string PlayerTextureFile = ASSET_PATH "Walpurgia.png";
		createTextureImage(PlayerTextureFile.c_str(), m_sceneRessources.playerTextureImage,
			m_sceneRessources.playerTextureImageMemory);
		m_sceneRessources.playerTextureImageView = createImageView(m_sceneRessources.playerTextureImage,
			VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_ASPECT_COLOR_BIT);
	}
}

void Renderer3D::createTextureSampler()
{
	VkSamplerCreateInfo samplerInfo{};
	samplerInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
	samplerInfo.magFilter = VK_FILTER_NEAREST;
	samplerInfo.minFilter = VK_FILTER_NEAREST;
	samplerInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
	samplerInfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;
	samplerInfo.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT;
	samplerInfo.anisotropyEnable = VK_TRUE;

	VkPhysicalDeviceProperties properties{};
	vkGetPhysicalDeviceProperties(m_physicalDevice, &properties);
	samplerInfo.maxAnisotropy = properties.limits.maxSamplerAnisotropy;
	samplerInfo.borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK;
	samplerInfo.unnormalizedCoordinates = VK_FALSE;
	samplerInfo.compareEnable = VK_FALSE;
	samplerInfo.compareOp = VK_COMPARE_OP_ALWAYS;
	samplerInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_NEAREST;
	samplerInfo.mipLodBias = 0.0f;
	samplerInfo.minLod = 0.0f;
	samplerInfo.maxLod = 0.0f;
	if (vkCreateSampler(m_device, &samplerInfo, nullptr, &m_textureSamplerNearest) != VK_SUCCESS)
		throw std::runtime_error("VUlkan: failed to create texture sampler!");
}

void Renderer3D::createVertexAndIndexBuffers()
{
	// Static tile vertex buffer creation
	for (size_t i = 0; i < m_activeScene->m_cellGrid.size(); i++)
	{
		const Cell& cell = m_activeScene->m_cellGrid[i];
		for (size_t j = 0; j < cell.m_staticTiles.size(); j++)
		{
			const Tile& staticTile = cell.m_staticTiles[j];
			auto spriteTexCoords = queryStaticTileTextureCoords(staticTile.m_spriteIndex, staticTile.m_rotation);

			StaticTileVertex vertexBottomLeft;
			vertexBottomLeft.worldPos.x = staticTile.m_gridLocation.x + (float)cell.cellPosition[0];
			vertexBottomLeft.worldPos.y = staticTile.m_gridLocation.y + (float)cell.cellPosition[1];
			vertexBottomLeft.worldPos.z = staticTile.m_gridLocation.z;
			vertexBottomLeft.texCoord = spriteTexCoords[0];
			
			StaticTileVertex vertexBottomRight;
			vertexBottomRight.worldPos.x = 1.0f + staticTile.m_gridLocation.x + (float)cell.cellPosition[0];
			vertexBottomRight.worldPos.y = staticTile.m_gridLocation.y + (float)cell.cellPosition[1];
			vertexBottomRight.worldPos.z = staticTile.m_gridLocation.z;
			vertexBottomRight.texCoord = spriteTexCoords[1];

			StaticTileVertex vertexTopRight;
			vertexTopRight.worldPos.x = 1.0f + staticTile.m_gridLocation.x + (float)cell.cellPosition[0];
			vertexTopRight.worldPos.y = 1.0f + staticTile.m_gridLocation.y + (float)cell.cellPosition[1];
			vertexTopRight.worldPos.z = staticTile.m_gridLocation.z;
			vertexTopRight.texCoord = spriteTexCoords[2];

			StaticTileVertex vertexTopLeft;
			vertexTopLeft.worldPos.x = staticTile.m_gridLocation.x + (float)cell.cellPosition[0];
			vertexTopLeft.worldPos.y = 1.0f + staticTile.m_gridLocation.y + (float)cell.cellPosition[1];
			vertexTopLeft.worldPos.z = staticTile.m_gridLocation.z;
			vertexTopLeft.texCoord = spriteTexCoords[3];

			size_t numVerticesBefore = m_sceneRessources.staticTileVertices.size();
			m_sceneRessources.staticTileVertices.push_back(vertexBottomLeft);
			m_sceneRessources.staticTileVertices.push_back(vertexBottomRight);
			m_sceneRessources.staticTileVertices.push_back(vertexTopRight);
			m_sceneRessources.staticTileVertices.push_back(vertexTopLeft);
			m_sceneRessources.staticTileIndices.insert(m_sceneRessources.staticTileIndices.end(),
				{
				(uint16_t)(numVerticesBefore + 0),
				(uint16_t)(numVerticesBefore + 1),
				(uint16_t)(numVerticesBefore + 2),
				(uint16_t)(numVerticesBefore + 2),
				(uint16_t)(numVerticesBefore + 3),
				(uint16_t)(numVerticesBefore + 0)
				}
			);
		}
	}


	VkDeviceSize bufferSize = sizeof(StaticTileVertex) * m_sceneRessources.staticTileVertices.size();
	createVertexBuffer(bufferSize, m_sceneRessources.staticTileVertices.data(),
		m_sceneRessources.staticTileVertexBuffer, m_sceneRessources.staticTileVertexBufferMemory);
	bufferSize = sizeof(uint16_t) * m_sceneRessources.staticTileIndices.size();
	createIndexBuffer(bufferSize, m_sceneRessources.staticTileIndices.data(),
		m_sceneRessources.staticTileIndexBuffer, m_sceneRessources.staticTileIndexBufferMemory);
	
	// Player buffer creation
	{
		float offset = 0.7 / 16.0f;
		m_sceneRessources.playerVertices = {
			/* BottomLeft  */{{0.0f - offset, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f}, {0.0f, 0.5f}},
			/* BottomRight */{{2.0f - offset, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f}, {0.25f, 0.5f}},
			/* TopRight    */{{2.0f - offset, 0.0f, 2.0f}, {0.0f, 0.0f, 0.0f}, {0.25f, 0.0f}},
			/* TopLeft     */{{0.0f - offset, 0.0f, 2.0f}, {0.0f, 0.0f, 0.0f}, {0.0f, 0.0f}}
		};
		m_sceneRessources.playerIndices = { 0, 1, 2, 2, 3, 0 };
		
		VkDeviceSize bufferSize = sizeof(Vertex) * m_sceneRessources.playerVertices.size();
		createVertexBuffer(bufferSize, m_sceneRessources.playerVertices.data(),
			m_sceneRessources.playerVertexBuffer, m_sceneRessources.playerVertexBufferMemory);
		bufferSize = sizeof(uint16_t) * m_sceneRessources.playerIndices.size();
		createIndexBuffer(bufferSize, m_sceneRessources.playerIndices.data(),
			m_sceneRessources.playerIndexBuffer, m_sceneRessources.playerIndexBufferMemory);
	}
}

void Renderer3D::createUniformBuffers()
{
	// Global Uniform Buffers
	{
		VkDeviceSize bufferSize = sizeof(UniformBufferCameraObject);
		m_sceneRessources.globalUniformBuffers.resize(MAX_FRAMES_IN_FLIGHT);
		m_sceneRessources.globalUniformBuffersMemory.resize(MAX_FRAMES_IN_FLIGHT);
		m_sceneRessources.globalUniformBuffersMapped.resize(MAX_FRAMES_IN_FLIGHT);

		for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
		{
			createBuffer(bufferSize, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
				VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
				m_sceneRessources.globalUniformBuffers[i], m_sceneRessources.globalUniformBuffersMemory[i]);
			// Persistent Mapping
			vkMapMemory(m_device, m_sceneRessources.globalUniformBuffersMemory[i], 0, bufferSize, 
				0, &m_sceneRessources.globalUniformBuffersMapped[i]);
		}
	}
}

void Renderer3D::createDescriptorPool()
{
	std::array<VkDescriptorPoolSize, 2> poolSizes{};
	poolSizes[0].type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	poolSizes[0].descriptorCount = (uint32_t)MAX_FRAMES_IN_FLIGHT;
	poolSizes[1].type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
	poolSizes[1].descriptorCount = (uint32_t)MAX_FRAMES_IN_FLIGHT;

	VkDescriptorPoolCreateInfo poolInfo{};
	poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
	poolInfo.poolSizeCount = (uint32_t)poolSizes.size();
	poolInfo.pPoolSizes = poolSizes.data();
	poolInfo.maxSets = 2 * (uint32_t)MAX_FRAMES_IN_FLIGHT; // 2* Because currently uses sets global and object specific set.
	if (vkCreateDescriptorPool(m_device, &poolInfo, nullptr, &m_descriptorPool) != VK_SUCCESS)
		throw std::runtime_error("Vulkan: failed to create descriptor pool!");
}

void Renderer3D::createDescriptorSets()
{
	// global Descriptor Set
	{
		std::vector<VkDescriptorSetLayout> layouts(MAX_FRAMES_IN_FLIGHT, m_sceneRessources.globalDescriptorSetLayout);
		VkDescriptorSetAllocateInfo allocInfo{};
		allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
		allocInfo.descriptorPool = m_descriptorPool;
		allocInfo.descriptorSetCount = (uint32_t)MAX_FRAMES_IN_FLIGHT;
		allocInfo.pSetLayouts = layouts.data();

		m_sceneRessources.globalDescriptorSets.resize(MAX_FRAMES_IN_FLIGHT);
		if (vkAllocateDescriptorSets(m_device, &allocInfo, m_sceneRessources.globalDescriptorSets.data()) != VK_SUCCESS)
			throw std::runtime_error("Vulkan: failed to create descriptor sets!");

		for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
		{
			VkDescriptorBufferInfo bufferInfo{};
			bufferInfo.buffer = m_sceneRessources.globalUniformBuffers[i];
			bufferInfo.offset = 0;
			bufferInfo.range = sizeof(UniformBufferCameraObject);

			VkWriteDescriptorSet descriptorWrite{};
			descriptorWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
			descriptorWrite.dstSet = m_sceneRessources.globalDescriptorSets[i];
			descriptorWrite.dstBinding = 0;
			descriptorWrite.dstArrayElement = 0;
			descriptorWrite.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
			descriptorWrite.descriptorCount = 1;
			descriptorWrite.pBufferInfo = &bufferInfo;

			vkUpdateDescriptorSets(m_device, 1, &descriptorWrite, 0, nullptr);
		}
	}
	
	// Static Tile Descriptor Set
	{
		std::vector<VkDescriptorSetLayout> layouts(MAX_FRAMES_IN_FLIGHT, m_sceneRessources.staticTileDescriptorSetLayout);
		VkDescriptorSetAllocateInfo allocInfo{};
		allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
		allocInfo.descriptorPool = m_descriptorPool;
		allocInfo.descriptorSetCount = (uint32_t)MAX_FRAMES_IN_FLIGHT;
		allocInfo.pSetLayouts = layouts.data();

		m_sceneRessources.staticTileDescriptorSets.resize(MAX_FRAMES_IN_FLIGHT);
		if (vkAllocateDescriptorSets(m_device, &allocInfo, m_sceneRessources.staticTileDescriptorSets.data()) != VK_SUCCESS)
			throw std::runtime_error("Vulkan: failed to create descriptor sets!");

		VkDescriptorImageInfo staticTileTextureImageInfo{};
		staticTileTextureImageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
		staticTileTextureImageInfo.imageView = m_sceneRessources.staticTileTextureImageView;
		staticTileTextureImageInfo.sampler = m_textureSamplerNearest;

		for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
		{
			VkWriteDescriptorSet descriptorWrite{};
			descriptorWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
			descriptorWrite.dstSet = m_sceneRessources.staticTileDescriptorSets[i];
			descriptorWrite.dstBinding = 0;
			descriptorWrite.dstArrayElement = 0;
			descriptorWrite.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
			descriptorWrite.descriptorCount = 1;
			descriptorWrite.pImageInfo = &staticTileTextureImageInfo;

			vkUpdateDescriptorSets(m_device, 1, &descriptorWrite, 0, nullptr);
		}
	}
}

void Renderer3D::createCommandBuffers()
{
	m_commandBuffers.resize(MAX_FRAMES_IN_FLIGHT);

	VkCommandBufferAllocateInfo allocInfo{};
	allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	allocInfo.commandPool = m_commandPool;
	allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	allocInfo.commandBufferCount = (uint32_t)m_commandBuffers.size();
	if (vkAllocateCommandBuffers(m_device, &allocInfo, m_commandBuffers.data()) != VK_SUCCESS)
		throw std::runtime_error("VK: failed to create command buffers!");
}

void Renderer3D::createSyncObjects()
{
	m_imageAvailableSemaphores.resize(MAX_FRAMES_IN_FLIGHT);
	m_renderFinishedSemaphores.resize(MAX_FRAMES_IN_FLIGHT);
	m_inFlightFences.resize(MAX_FRAMES_IN_FLIGHT);

	VkSemaphoreCreateInfo semaphoreInfo{};
	semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
	VkFenceCreateInfo fenceInfo{};
	fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
	fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;
	for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
	{
		if (vkCreateSemaphore(m_device, &semaphoreInfo, nullptr, &m_imageAvailableSemaphores[i]) != VK_SUCCESS
			|| vkCreateSemaphore(m_device, &semaphoreInfo, nullptr, &m_renderFinishedSemaphores[i]) != VK_SUCCESS
			|| vkCreateFence(m_device, &fenceInfo, nullptr, &m_inFlightFences[i]) != VK_SUCCESS)
			throw std::runtime_error("VK: failed to create sync objects!");
	}
}

void Renderer3D::recreateSwapChain()
{
	// Handle window minimization by doing nothing in that time
	int width = 0, height = 0;
	glfwGetFramebufferSize(Game::getInstance().getWindow(), &width, &height);
	while (width == 0 || height == 0)
	{
		glfwGetFramebufferSize(Game::getInstance().getWindow(), &width, &height);
		glfwWaitEvents();
	}

	// This implementation requires rendering to finish completely
	vkDeviceWaitIdle(m_device);

	cleanupSwapChain();

	createSwapChain();
	createImageViews();
	createDepthRessources();
	createFramebuffers();
}

void Renderer3D::cleanupSwapChain()
{
	for (size_t i = 0; i < m_depthImages.size(); i++)
	{
		vkDestroyImageView(m_device, m_depthImageViews[i], nullptr);
		vkDestroyImage(m_device, m_depthImages[i], nullptr);
		vkFreeMemory(m_device, m_depthImageMemories[i], nullptr);
	}
	for (size_t i = 0; i < m_swapChainFramebuffers.size(); i++)
	{
		vkDestroyFramebuffer(m_device, m_swapChainFramebuffers[i], nullptr);
	}
	for (size_t i = 0; i < m_swapChainImageViews.size(); i++)
	{
		vkDestroyImageView(m_device, m_swapChainImageViews[i], nullptr);
	}
	vkDestroySwapchainKHR(m_device, m_swapChain, nullptr);
}

void Renderer3D::cleanupSceneRessources()
{
	vkDestroySampler(m_device, m_textureSamplerNearest, nullptr);

	// Cleanup static tile ressources
	vkDestroyImageView(m_device, m_sceneRessources.staticTileTextureImageView, nullptr);
	vkDestroyImage(m_device, m_sceneRessources.staticTileTextureImage, nullptr);
	vkFreeMemory(m_device, m_sceneRessources.staticTileTextureImageMemory, nullptr);

	vkDestroyBuffer(m_device, m_sceneRessources.staticTileVertexBuffer, nullptr);
	vkFreeMemory(m_device, m_sceneRessources.staticTileVertexBufferMemory, nullptr);
	vkDestroyBuffer(m_device, m_sceneRessources.staticTileIndexBuffer, nullptr);
	vkFreeMemory(m_device, m_sceneRessources.staticTileIndexBufferMemory, nullptr);

	// Cleanup player ressources
	vkDestroyImageView(m_device, m_sceneRessources.playerTextureImageView, nullptr);
	vkDestroyImage(m_device, m_sceneRessources.playerTextureImage, nullptr);
	vkFreeMemory(m_device, m_sceneRessources.playerTextureImageMemory, nullptr);

	vkDestroyBuffer(m_device, m_sceneRessources.playerVertexBuffer, nullptr);
	vkFreeMemory(m_device, m_sceneRessources.playerVertexBufferMemory, nullptr);
	vkDestroyBuffer(m_device, m_sceneRessources.playerIndexBuffer, nullptr);
	vkFreeMemory(m_device, m_sceneRessources.playerIndexBufferMemory, nullptr);

	vkDestroyDescriptorPool(m_device, m_descriptorPool, nullptr);
	vkDestroyDescriptorSetLayout(m_device, m_sceneRessources.globalDescriptorSetLayout, nullptr);
	vkDestroyDescriptorSetLayout(m_device, m_sceneRessources.staticTileDescriptorSetLayout, nullptr);

	for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
	{
		vkDestroyBuffer(m_device, m_sceneRessources.globalUniformBuffers[i], nullptr);
		vkFreeMemory(m_device, m_sceneRessources.globalUniformBuffersMemory[i], nullptr);
	}

	vkDestroyPipeline(m_device, m_staticPipelineRes.graphicsPipeline, nullptr);
	vkDestroyPipelineLayout(m_device, m_staticPipelineRes.pipelineLayout, nullptr);
	vkDestroyPipeline(m_device, m_actorPipelineRes.graphicsPipeline, nullptr);
	vkDestroyPipelineLayout(m_device, m_actorPipelineRes.pipelineLayout, nullptr);

}

//
// Helper Functions
//

std::vector<char> Renderer3D::readShaderFromFile(const std::string& filename)
{
	std::ifstream file(filename, std::ios::ate | std::ios::binary);
	if (!file.is_open())
		throw std::runtime_error("Utility: failed to read file" + filename);
	size_t fileSize = (size_t)file.tellg();
	std::vector<char> buffer(fileSize);
	file.seekg(0);
	file.read(buffer.data(), fileSize);
	file.close();

	return buffer;
}

VkShaderModule Renderer3D::createShaderModule(const std::vector<char>& code)
{
	VkShaderModuleCreateInfo createInfo{};
	createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
	createInfo.codeSize = code.size();
	createInfo.pCode = reinterpret_cast<const uint32_t*>(code.data());

	VkShaderModule shaderModule;
	if (vkCreateShaderModule(m_device, &createInfo, nullptr, &shaderModule) != VK_SUCCESS)
		throw std::runtime_error("Vulkan: failed to create shader module");
	return shaderModule;
}

void Renderer3D::createGraphicsPipeline(const std::string& i_vertShaderFilename, const std::string& i_fragShaderFilename,
	VkPushConstantRange* i_pushConstantRange, GraphicsPipelineRessources& pipelineRessources)
{
	auto vertShaderCode = readShaderFromFile(i_vertShaderFilename);
	auto fragShaderCode = readShaderFromFile(i_fragShaderFilename);
#ifdef _DEBUG
	std::cout << "Size of static tile vert shader: " << vertShaderCode.size() << " bytes"
		<< "\nSize of static tile frag shader: " << fragShaderCode.size() << " bytes" << std::endl;
#endif

	VkShaderModule vertShaderModule = createShaderModule(vertShaderCode);
	VkShaderModule fragShaderModule = createShaderModule(fragShaderCode);

	VkPipelineShaderStageCreateInfo vertShaderStageInfo{};
	vertShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
	vertShaderStageInfo.stage = VK_SHADER_STAGE_VERTEX_BIT;
	vertShaderStageInfo.module = vertShaderModule;
	vertShaderStageInfo.pName = "main"; //entrypoint
	//vertShaderStageInfo.pSpecializationInfo is used for specifying shader constants

	VkPipelineShaderStageCreateInfo fragShaderStageInfo{};
	fragShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
	fragShaderStageInfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
	fragShaderStageInfo.module = fragShaderModule;
	fragShaderStageInfo.pName = "main"; //entrypoint

	VkPipelineShaderStageCreateInfo shaderStages[] = { vertShaderStageInfo, fragShaderStageInfo };

	std::vector<VkDynamicState> dynamicStates = { VK_DYNAMIC_STATE_VIEWPORT, VK_DYNAMIC_STATE_SCISSOR };
	// Dynamic states able to be changed a t draw time without having to recreate the pipeline
	VkPipelineDynamicStateCreateInfo dynamicState{};
	dynamicState.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
	dynamicState.dynamicStateCount = (uint32_t)dynamicStates.size();
	dynamicState.pDynamicStates = dynamicStates.data();

	auto bindingDescriptions = StaticTileVertex::getBindingDescription();
	auto attributeDescriptions = StaticTileVertex::getAttributeDescriptions();

	VkPipelineVertexInputStateCreateInfo vertexInputInfo{};
	vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
	vertexInputInfo.vertexBindingDescriptionCount = 1;
	vertexInputInfo.pVertexBindingDescriptions = &bindingDescriptions;
	vertexInputInfo.vertexAttributeDescriptionCount = (uint32_t)attributeDescriptions.size();
	vertexInputInfo.pVertexAttributeDescriptions = attributeDescriptions.data();

	VkPipelineInputAssemblyStateCreateInfo inputAssembly{};
	inputAssembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
	inputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
	inputAssembly.primitiveRestartEnable = VK_FALSE;

	VkViewport viewport{};
	viewport.x = 0.0f;
	viewport.y = m_swapChainExtent.height;
	viewport.width = (float)m_swapChainExtent.width;
	viewport.height = -(float)m_swapChainExtent.height;
	viewport.minDepth = 0.0f;
	viewport.maxDepth = 1.0f; // min max must be within [0.0f, 1.0f]

	VkRect2D scissor{};
	scissor.offset = { 0, 0 };
	scissor.extent = m_swapChainExtent;

	VkPipelineViewportStateCreateInfo viewportState{};
	viewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
	viewportState.viewportCount = 1;
	viewportState.scissorCount = 1;

	VkPipelineRasterizationStateCreateInfo rasterizer{};
	rasterizer.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
	rasterizer.depthClampEnable = VK_FALSE;
	rasterizer.rasterizerDiscardEnable = VK_FALSE;
	rasterizer.polygonMode = VK_POLYGON_MODE_FILL;
	rasterizer.lineWidth = 1.0f;
	rasterizer.cullMode = VK_CULL_MODE_NONE;
	rasterizer.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;
	rasterizer.depthBiasEnable = VK_FALSE;

	VkPipelineMultisampleStateCreateInfo multisampling{};
	multisampling.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
	multisampling.sampleShadingEnable = VK_FALSE; // For later chapter
	multisampling.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;

	VkPipelineColorBlendAttachmentState colorBlendAttachment{};
	colorBlendAttachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT |
		VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
	colorBlendAttachment.blendEnable = VK_TRUE;
	colorBlendAttachment.srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA;
	colorBlendAttachment.dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
	colorBlendAttachment.colorBlendOp = VK_BLEND_OP_ADD;
	colorBlendAttachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE; // Optional
	colorBlendAttachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO; //	Optional
	colorBlendAttachment.alphaBlendOp = VK_BLEND_OP_ADD; // Optional

	VkPipelineColorBlendStateCreateInfo colorBlending{};
	colorBlending.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
	colorBlending.logicOpEnable = VK_FALSE;
	colorBlending.logicOp = VK_LOGIC_OP_COPY;
	colorBlending.attachmentCount = 1;
	colorBlending.pAttachments = &colorBlendAttachment;
	// Optional setting of constants here

	VkPipelineDepthStencilStateCreateInfo depthStencil{};
	depthStencil.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
	depthStencil.depthTestEnable = VK_TRUE;
	depthStencil.depthWriteEnable = VK_TRUE;
	depthStencil.depthCompareOp = VK_COMPARE_OP_LESS;
	depthStencil.depthBoundsTestEnable = VK_FALSE;
	depthStencil.minDepthBounds = 0.0f; // Optional
	depthStencil.maxDepthBounds = 1.0f; // Optional
	depthStencil.stencilTestEnable = VK_FALSE;
	depthStencil.front = {}; // Optional
	depthStencil.back = {}; // Optional

	VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
	pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
	std::array<VkDescriptorSetLayout, 2> descriptorSetLayouts =
		{ m_sceneRessources.globalDescriptorSetLayout, m_sceneRessources.staticTileDescriptorSetLayout };
	pipelineLayoutInfo.setLayoutCount = (uint32_t)descriptorSetLayouts.size();
	pipelineLayoutInfo.pSetLayouts = descriptorSetLayouts.data();
	if (i_pushConstantRange)
	{
		pipelineLayoutInfo.pushConstantRangeCount = 1;
		pipelineLayoutInfo.pPushConstantRanges = i_pushConstantRange;
	}
	else
	{
		pipelineLayoutInfo.pushConstantRangeCount = 0;
		pipelineLayoutInfo.pPushConstantRanges = nullptr;
	}

	if (vkCreatePipelineLayout(m_device, &pipelineLayoutInfo, nullptr, &pipelineRessources.pipelineLayout) != VK_SUCCESS)
		throw std::runtime_error("Vulkan: failed to create pipeline layout!");
	
	VkGraphicsPipelineCreateInfo pipelineInfo{};
	pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
	pipelineInfo.stageCount = 2;
	pipelineInfo.pStages = shaderStages;
	pipelineInfo.pVertexInputState = &vertexInputInfo;
	pipelineInfo.pInputAssemblyState = &inputAssembly;
	pipelineInfo.pViewportState = &viewportState;
	pipelineInfo.pRasterizationState = &rasterizer;
	pipelineInfo.pMultisampleState = &multisampling;
	pipelineInfo.pDepthStencilState = &depthStencil; // Optional
	pipelineInfo.pColorBlendState = &colorBlending;
	pipelineInfo.pDynamicState = &dynamicState;
	pipelineInfo.layout = pipelineRessources.pipelineLayout;
	pipelineInfo.renderPass = m_renderPass;
	pipelineInfo.subpass = 0;
	// The last two are used for when an already existing pipeline is used for the creation of a new one
	pipelineInfo.basePipelineHandle = VK_NULL_HANDLE; // Optional 
	pipelineInfo.basePipelineIndex = -1; // Optional
	if (vkCreateGraphicsPipelines(m_device, VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &pipelineRessources.graphicsPipeline) != VK_SUCCESS)
		throw std::runtime_error("VK: failed to create graphics pipeline");

	vkDestroyShaderModule(m_device, fragShaderModule, nullptr);
	vkDestroyShaderModule(m_device, vertShaderModule, nullptr);
}

void Renderer3D::recordCommandBuffer(VkCommandBuffer commandBuffer, uint32_t imageIndex)
{
	VkCommandBufferBeginInfo beginInfo{};
	beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
	beginInfo.flags = 0; // Optional
	beginInfo.pInheritanceInfo = nullptr; // Optional
	if (vkBeginCommandBuffer(commandBuffer, &beginInfo) != VK_SUCCESS)
		throw std::runtime_error("VK: failed to begin record command buffer!");

	std::array<VkClearValue, 2> clearValues{};
	clearValues[0].color = { {0.0f, 0.0f, 0.0f, 1.0f} }; // Black clear color
	clearValues[1].depthStencil = { 1.0f, 0 }; //default depth value = 1.0f -> furthest

	VkRenderPassBeginInfo renderPassInfo{};
	renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
	renderPassInfo.renderPass = m_renderPass;
	renderPassInfo.framebuffer = m_swapChainFramebuffers[imageIndex];
	renderPassInfo.renderArea.offset = { 0, 0 };
	renderPassInfo.renderArea.extent = m_swapChainExtent;
	renderPassInfo.clearValueCount = (uint32_t)clearValues.size();
	renderPassInfo.pClearValues = clearValues.data();

	vkCmdBeginRenderPass(commandBuffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);
	
	VkViewport viewport{};
	viewport.x = 0.0f;
	viewport.y = 0.0f;
	viewport.width = (float)m_swapChainExtent.width;
	viewport.height = (float)m_swapChainExtent.height;
	viewport.minDepth = 0.0f;
	viewport.maxDepth = 1.0f;
	vkCmdSetViewport(commandBuffer, 0, 1, &viewport);

	VkRect2D scissor{};
	scissor.offset = { 0, 0 };
	scissor.extent = m_swapChainExtent;
	vkCmdSetScissor(commandBuffer, 0, 1, &scissor);

	/*
	Draw static objects with the static object pipeline
	*/
	{
		vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, m_staticPipelineRes.graphicsPipeline);
		VkBuffer vertexBuffers[] = { m_sceneRessources.staticTileVertexBuffer };
		VkDeviceSize offsets[] = { 0 };
		vkCmdBindVertexBuffers(commandBuffer, 0, 1, vertexBuffers, offsets);
		vkCmdBindIndexBuffer(commandBuffer, m_sceneRessources.staticTileIndexBuffer, 0, VK_INDEX_TYPE_UINT16);
		// Bind descriptor sets (Global is set zero, object related stuff is set one)
		std::array<VkDescriptorSet, 2> descriptorSetsToBind =
			{ m_sceneRessources.globalDescriptorSets[m_currentFrame], 
			m_sceneRessources.staticTileDescriptorSets[m_currentFrame] };
		vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, m_staticPipelineRes.pipelineLayout,
			0, 2, descriptorSetsToBind.data(), 0, nullptr);
		vkCmdDrawIndexed(commandBuffer, (uint32_t)m_sceneRessources.staticTileIndices.size(), 1, 0, 0, 0);
	}

	/*
	Draw actors with the actors with the actor object pipeline
	player, enemies and actor objects are actors
	*/
	{
		vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, m_actorPipelineRes.graphicsPipeline);
		ModelMatrixPushConstant playerPushConstants{};
		playerPushConstants.translate = m_activeScene->m_player.m_position;
		playerPushConstants.rotate = m_activeScene->m_player.m_orientation;
		vkCmdPushConstants(commandBuffer, m_actorPipelineRes.pipelineLayout, VK_SHADER_STAGE_VERTEX_BIT, 0,
			sizeof(ModelMatrixPushConstant), &playerPushConstants);
		VkBuffer vertexBuffers[] = { m_sceneRessources.playerVertexBuffer };
		VkDeviceSize offsets[] = { 0 };
		vkCmdBindVertexBuffers(commandBuffer, 0, 1, vertexBuffers, offsets);
		vkCmdBindIndexBuffer(commandBuffer, m_sceneRessources.playerIndexBuffer, 0, VK_INDEX_TYPE_UINT16);
		// Bind descriptor sets (Global is set zero, object related stuff is set one)
		// However if another actor is drawn only object related set has to be bound again.
		std::array<VkDescriptorSet, 2> descriptorSetsToBind =
		{ m_sceneRessources.globalDescriptorSets[m_currentFrame],
		m_sceneRessources.staticTileDescriptorSets[m_currentFrame] };
		vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, m_actorPipelineRes.pipelineLayout,
			0, 2, descriptorSetsToBind.data(), 0, nullptr);
		vkCmdDrawIndexed(commandBuffer, (uint32_t)m_sceneRessources.playerIndices.size(), 1, 0, 0, 0);
	}

	vkCmdEndRenderPass(commandBuffer);
	if (vkEndCommandBuffer(commandBuffer) != VK_SUCCESS)
		throw std::runtime_error("VK: failed to record command buffer!");
}

VkCommandBuffer Renderer3D::beginSingleTimeCommands()
{
	VkCommandBufferAllocateInfo allocInfo{};
	allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	allocInfo.commandPool = m_commandPool;
	allocInfo.commandBufferCount = 1;

	VkCommandBuffer commandBuffer;
	vkAllocateCommandBuffers(m_device, &allocInfo, &commandBuffer);

	VkCommandBufferBeginInfo beginInfo{};
	beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
	beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

	vkBeginCommandBuffer(commandBuffer, &beginInfo);

	return commandBuffer;
}

void Renderer3D::endSingleTimeCommands(VkCommandBuffer commandBuffer)
{
	// End recording
	vkEndCommandBuffer(commandBuffer);

	VkSubmitInfo submitInfo{};
	submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
	submitInfo.commandBufferCount = 1;
	submitInfo.pCommandBuffers = &commandBuffer;

	vkQueueSubmit(m_graphicsQueue, 1, &submitInfo, VK_NULL_HANDLE);
	vkQueueWaitIdle(m_graphicsQueue); // Or create fence and wait for that fence this might be otherwise a big bottleneck
	vkFreeCommandBuffers(m_device, m_commandPool, 1, &commandBuffer);
}

uint32_t Renderer3D::findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties)
{
	VkPhysicalDeviceMemoryProperties memProperties;
	vkGetPhysicalDeviceMemoryProperties(m_physicalDevice, &memProperties);
	for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++)
	{
		if (typeFilter & (1 << i)
			&& (memProperties.memoryTypes[i].propertyFlags & properties) == properties)
		{
			return i;
		}
	}

	throw std::runtime_error("Vulkan: failed to find suitable memory type!");
}

void Renderer3D::createBuffer(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties,
	VkBuffer& buffer, VkDeviceMemory& bufferMemory)
{
	VkBufferCreateInfo bufferInfo{};
	bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
	bufferInfo.size = size;
	bufferInfo.usage = usage;
	bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
	if (vkCreateBuffer(m_device, &bufferInfo, nullptr, &buffer) != VK_SUCCESS)
		throw std::runtime_error("Vulkan: failed to create vertex buffer!");

	VkMemoryRequirements memRequirements;
	vkGetBufferMemoryRequirements(m_device, buffer, &memRequirements);

	VkMemoryAllocateInfo allocInfo{};
	allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
	allocInfo.allocationSize = memRequirements.size;
	allocInfo.memoryTypeIndex = findMemoryType(memRequirements.memoryTypeBits, properties);
	if (vkAllocateMemory(m_device, &allocInfo, nullptr, &bufferMemory) != VK_SUCCESS)
		throw std::runtime_error("Vulkan: failed to allocate vertex buffer memory!");
	vkBindBufferMemory(m_device, buffer, bufferMemory, 0);
}

void Renderer3D::copyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size)
{
	// Needs temporary command buffer and is recommended to use separate command pool 
	// somewhere in memory with the flag VK_COMMAND_POOL_CREATE_TRANSIENT_BIT
	VkCommandBuffer commandBuffer = beginSingleTimeCommands();

	VkBufferCopy copyRegion{};
	copyRegion.srcOffset = 0; // Optional
	copyRegion.dstOffset = 0; // Optional
	copyRegion.size = size;

	vkCmdCopyBuffer(commandBuffer, srcBuffer, dstBuffer, 1, &copyRegion);

	endSingleTimeCommands(commandBuffer);
}

void Renderer3D::createImage(uint32_t width, uint32_t height, VkFormat format, VkImageTiling tiling,
	VkImageUsageFlags usage, VkMemoryPropertyFlags properties, VkImage& image, VkDeviceMemory& imageMemory)
{
	VkImageCreateInfo imageInfo{};
	imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
	imageInfo.imageType = VK_IMAGE_TYPE_2D;
	imageInfo.extent.width = (uint32_t)width;
	imageInfo.extent.height = (uint32_t)height;
	imageInfo.extent.depth = 1;
	imageInfo.mipLevels = 1;
	imageInfo.arrayLayers = 1;
	imageInfo.format = format;
	imageInfo.tiling = tiling;
	imageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	imageInfo.usage = usage;
	imageInfo.samples = VK_SAMPLE_COUNT_1_BIT;
	imageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
	imageInfo.flags = 0; // Optional
	if (vkCreateImage(m_device, &imageInfo, nullptr, &image) != VK_SUCCESS)
		throw std::runtime_error("Vulkan: failed to create image!");

	VkMemoryRequirements memRequirements;
	vkGetImageMemoryRequirements(m_device, image, &memRequirements);

	VkMemoryAllocateInfo allocInfo{};
	allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
	allocInfo.allocationSize = memRequirements.size;
	allocInfo.memoryTypeIndex = findMemoryType(memRequirements.memoryTypeBits,
		VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
	if (vkAllocateMemory(m_device, &allocInfo, nullptr, &imageMemory) != VK_SUCCESS)
		throw std::runtime_error("Vulkan: failed to allocate image memory!");

	vkBindImageMemory(m_device, image, imageMemory, 0);
}

void Renderer3D::transitionImageLayout(VkImage image, VkFormat format, VkImageLayout oldLayout, VkImageLayout newLayout)
{
	VkImageMemoryBarrier barrier{};
	barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
	barrier.oldLayout = oldLayout;
	barrier.newLayout = newLayout;
	barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	barrier.image = image;

	if (newLayout == VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL)
	{
		barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;
		if (hasStencilComponent(format))
			barrier.subresourceRange.aspectMask |= VK_IMAGE_ASPECT_STENCIL_BIT;

	}
	else {
		barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
	}

	barrier.subresourceRange.baseMipLevel = 0;
	barrier.subresourceRange.levelCount = 1;
	barrier.subresourceRange.baseArrayLayer = 0;
	barrier.subresourceRange.layerCount = 1;

	VkPipelineStageFlags sourceStage;
	VkPipelineStageFlags destinationStage;

	if (oldLayout == VK_IMAGE_LAYOUT_UNDEFINED
		&& newLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL)
	{
		barrier.srcAccessMask = 0;
		barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;

		sourceStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
		destinationStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
	}
	else if (oldLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL
		&& newLayout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL)
	{
		barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
		barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

		sourceStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
		destinationStage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
	}
	else if (oldLayout == VK_IMAGE_LAYOUT_UNDEFINED
		&& newLayout == VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL)
	{
		barrier.srcAccessMask = 0;
		barrier.dstAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT
			| VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;

		sourceStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
		destinationStage = VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
	}
	else {
		throw std::invalid_argument("Vulkan: unsupported layer transition!");
	}

	VkCommandBuffer commandBuffer = beginSingleTimeCommands();

	vkCmdPipelineBarrier(commandBuffer,
		sourceStage, destinationStage,
		0,
		0, nullptr,
		0, nullptr,
		1, &barrier);

	endSingleTimeCommands(commandBuffer);
}

void Renderer3D::copyBufferToImage(VkBuffer buffer, VkImage image, uint32_t width, uint32_t height)
{
	VkBufferImageCopy region{};
	region.bufferOffset = 0;
	region.bufferRowLength = 0;
	region.bufferImageHeight = 0;
	region.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
	region.imageSubresource.mipLevel = 0;
	region.imageSubresource.baseArrayLayer = 0;
	region.imageSubresource.layerCount = 1;
	region.imageOffset = { 0, 0, 0 };
	region.imageExtent = { width, height, 1 };

	VkCommandBuffer commandBuffer = beginSingleTimeCommands();

	vkCmdCopyBufferToImage(commandBuffer, buffer, image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &region);

	endSingleTimeCommands(commandBuffer);
}

VkImageView Renderer3D::createImageView(VkImage image, VkFormat format, VkImageAspectFlags aspectFlags)
{
	VkImageViewCreateInfo createInfo{};
	createInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
	createInfo.image = image;
	createInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
	createInfo.format = format;
	createInfo.subresourceRange.aspectMask = aspectFlags;
	createInfo.subresourceRange.baseMipLevel = 0;
	createInfo.subresourceRange.levelCount = 1;
	createInfo.subresourceRange.baseArrayLayer = 0;
	createInfo.subresourceRange.layerCount = 1;

	VkImageView imageView;
	if (vkCreateImageView(m_device, &createInfo, nullptr, &imageView) != VK_SUCCESS)
	{
		throw std::runtime_error("Vulkan: failed to create image view!");
	}

	return imageView;
}

VkFormat Renderer3D::findSupportedFormat(const std::vector<VkFormat>& candidates, VkImageTiling tiling,
	VkFormatFeatureFlags features)
{
	for (VkFormat format : candidates)
	{
		VkFormatProperties props;
		vkGetPhysicalDeviceFormatProperties(m_physicalDevice, format, &props);
		if (tiling == VK_IMAGE_TILING_LINEAR && (props.linearTilingFeatures & features) == features)
			return format;
		else if (tiling == VK_IMAGE_TILING_OPTIMAL && (props.optimalTilingFeatures & features) == features)
			return format;
	}

	throw std::runtime_error("Vulkan: failed to find a supported format!");
}

VkFormat Renderer3D::findDepthFormat()
{
	return findSupportedFormat(
		{ VK_FORMAT_D32_SFLOAT, VK_FORMAT_D32_SFLOAT_S8_UINT,
		VK_FORMAT_D24_UNORM_S8_UINT },
		VK_IMAGE_TILING_OPTIMAL, VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT
	);
}

bool Renderer3D::hasStencilComponent(VkFormat format)
{
	return format == VK_FORMAT_D32_SFLOAT_S8_UINT || format == VK_FORMAT_D24_UNORM_S8_UINT;
}

void Renderer3D::createTextureImage(const char* textureFile, VkImage& textureImage, VkDeviceMemory& textureImageMemory)
{
	int texWidth, texHeight, texChannels;
	stbi_uc* pixels = stbi_load(textureFile, &texWidth, &texHeight, &texChannels, STBI_rgb_alpha);
	VkDeviceSize imageSize = texWidth * texHeight * 4;
	if (!pixels)
		throw std::runtime_error("STB: failed to load texture image!");

	VkBuffer stagingBuffer;
	VkDeviceMemory stagingBufferMemory;
	createBuffer(imageSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
		VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
		stagingBuffer, stagingBufferMemory);

	void* data;
	vkMapMemory(m_device, stagingBufferMemory, 0, imageSize, 0, &data);
	memcpy(data, pixels, (size_t)imageSize);
	vkUnmapMemory(m_device, stagingBufferMemory);
	stbi_image_free(pixels);

	createImage(texWidth, texHeight, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_TILING_OPTIMAL,
		VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
		textureImage, textureImageMemory);
	transitionImageLayout(textureImage, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_LAYOUT_UNDEFINED,
		VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);
	copyBufferToImage(stagingBuffer, textureImage, (uint32_t)texWidth, (uint32_t)texHeight);
	transitionImageLayout(textureImage, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
		VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);

	vkDestroyBuffer(m_device, stagingBuffer, nullptr);
	vkFreeMemory(m_device, stagingBufferMemory, nullptr);
}

void Renderer3D::createVertexBuffer(VkDeviceSize bufferSize, void* verticesData, VkBuffer& vertexBuffer, 
	VkDeviceMemory& vertexBufferMemory)
{
	// Important note: vkAllocateMemory to allocate memory in the GPU should not be used on individual buffers
	// but rather multiple buffers should be placed into one with offsets
	VkBuffer stagingBuffer;
	VkDeviceMemory stagingBufferMemory;
	createBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
		VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
		stagingBuffer, stagingBufferMemory);

	void* data;
	vkMapMemory(m_device, stagingBufferMemory, 0, bufferSize, 0, &data);
	memcpy(data, verticesData, (size_t)bufferSize);
	vkUnmapMemory(m_device, stagingBufferMemory);

	createBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
		VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, vertexBuffer, vertexBufferMemory);

	copyBuffer(stagingBuffer, vertexBuffer, bufferSize);

	vkDestroyBuffer(m_device, stagingBuffer, nullptr);
	vkFreeMemory(m_device, stagingBufferMemory, nullptr);
}

void Renderer3D::createIndexBuffer(VkDeviceSize bufferSize, void* indexData, VkBuffer& indexBuffer, 
	VkDeviceMemory& indexBufferMemory)
{
	VkBuffer stagingBuffer;
	VkDeviceMemory stagingBufferMemory;
	createBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
		VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
		stagingBuffer, stagingBufferMemory);

	void* data;
	vkMapMemory(m_device, stagingBufferMemory, 0, bufferSize, 0, &data);
	memcpy(data, indexData, (size_t)bufferSize);
	vkUnmapMemory(m_device, stagingBufferMemory);

	createBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT,
		VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, indexBuffer, indexBufferMemory);

	copyBuffer(stagingBuffer, indexBuffer, bufferSize);

	vkDestroyBuffer(m_device, stagingBuffer, nullptr);
	vkFreeMemory(m_device, stagingBufferMemory, nullptr);
}

std::array<glm::vec2, 4> Renderer3D::queryStaticTileTextureCoords(int index, int rotation)
{
	// TODO
	// For now only reads first row needs more implementation
	std::array<glm::vec2, 4> result;
	result[rotation % 4] = glm::vec2(0.1f * (float)index, 0.1f);
	result[(rotation + 1) % 4] = glm::vec2(0.1f * (float)index + 0.1f, 0.1f);
	result[(rotation + 2) % 4] = glm::vec2(0.1f * (float)index + 0.1f, 0.0f);
	result[(rotation + 3) % 4] = glm::vec2(0.1f * (float)index, 0.0f);
	return result;
}

//
// Main Loop
//

void Renderer3D::drawFrame()
{
	vkWaitForFences(m_device, 1, &m_inFlightFences[m_currentFrame], VK_TRUE, UINT64_MAX);

	uint32_t imageIndex;
	VkResult result = vkAcquireNextImageKHR(m_device, m_swapChain, UINT64_MAX,
		m_imageAvailableSemaphores[m_currentFrame], VK_NULL_HANDLE, &imageIndex);

	if (result == VK_ERROR_OUT_OF_DATE_KHR)
	{
		recreateSwapChain();
		return; // This skips a frame which is interesting probably better handled in an OnResize funktion before this call.
	}
	else if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR)
	{
		throw std::runtime_error("VK: failed to acquire swap chain image!");
	}

	// Only reset if work is submitted to avoid deadlock
	vkResetFences(m_device, 1, &m_inFlightFences[m_currentFrame]);

	vkResetCommandBuffer(m_commandBuffers[m_currentFrame], 0);
	recordCommandBuffer(m_commandBuffers[m_currentFrame], imageIndex);

	updateUniformBuffer(m_currentFrame);

	VkSubmitInfo submitInfo{};
	submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

	VkSemaphore waitSemaphore[] = { m_imageAvailableSemaphores[m_currentFrame] };
	VkPipelineStageFlags waitStages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
	submitInfo.waitSemaphoreCount = 1;
	submitInfo.pWaitSemaphores = waitSemaphore;
	submitInfo.pWaitDstStageMask = waitStages;
	submitInfo.commandBufferCount = 1;
	submitInfo.pCommandBuffers = &m_commandBuffers[m_currentFrame];

	VkSemaphore signalSemaphores[] = { m_renderFinishedSemaphores[m_currentFrame] };
	submitInfo.signalSemaphoreCount = 1;
	submitInfo.pSignalSemaphores = signalSemaphores;
	if (vkQueueSubmit(m_graphicsQueue, 1, &submitInfo, m_inFlightFences[m_currentFrame]) != VK_SUCCESS)
		throw std::runtime_error("VK: failed to submit draw command buffer!");

	VkPresentInfoKHR presentInfo{};
	presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
	presentInfo.waitSemaphoreCount = 1;
	presentInfo.pWaitSemaphores = signalSemaphores;
	VkSwapchainKHR swapChains[] = { m_swapChain };
	presentInfo.swapchainCount = 1;
	presentInfo.pSwapchains = swapChains;
	presentInfo.pImageIndices = &imageIndex;
	presentInfo.pResults = nullptr; // Optional
	result = vkQueuePresentKHR(m_presentQueue, &presentInfo);
	if (result == VK_ERROR_OUT_OF_DATE_KHR
		|| result == VK_SUBOPTIMAL_KHR
		|| m_framebufferResized)
	{
		m_framebufferResized = false;
		recreateSwapChain();
	}
	else if (result != VK_SUCCESS)
	{
		throw std::runtime_error("VK: failed to present swap chain image!");
	}

	m_currentFrame = (m_currentFrame + 1) % MAX_FRAMES_IN_FLIGHT;
}

void Renderer3D::updateUniformBuffer(uint32_t currentImage)
{
	// Not optimal for the purpose of small buffers -> use push constants instead
	static auto startTime = std::chrono::high_resolution_clock::now();

	auto currentTime = std::chrono::high_resolution_clock::now();
	float time = std::chrono::duration<float, std::chrono::seconds::period>
		(currentTime - startTime).count();

	UniformBufferCameraObject ubo{};
	// Rotation of the model around z-axis
	//ubo.model = glm::rotate(glm::mat4(1.0f), time * glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f));
	ubo.view = m_activeScene->m_activeCamera.getView();
	ubo.proj = m_activeScene->m_activeCamera.getProjection();
	ubo.proj[1][1] *= -1;
	memcpy(m_sceneRessources.globalUniformBuffersMapped[currentImage], &ubo, sizeof(ubo));
}