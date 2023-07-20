#include "Texture.h"

#include "Renderer3D.h"

Texture::Texture(Renderer3D& renderer) : r_renderer(renderer) {}

Texture::~Texture()
{
	vkDestroyImageView(r_renderer.m_device, imgView, nullptr);
	vkDestroyImage(r_renderer.m_device, img, nullptr);
	vkFreeMemory(r_renderer.m_device, memory, nullptr);
}