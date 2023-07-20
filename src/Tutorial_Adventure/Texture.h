#pragma once

#include <vulkan/vulkan.h>

class Renderer3D;

class Texture {
public:
	Texture(Renderer3D& renderer);
	~Texture();

	VkImage img;
	VkDeviceMemory memory;
	VkImageView imgView;

	void cleanup();
private:
	Renderer3D& r_renderer;
};
