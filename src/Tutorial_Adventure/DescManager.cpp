#include <stdexcept>
#include <iostream>

#include "DescManager.h"

#include "Renderer3D.h"

DescManager::DescManager(Renderer3D* renderer)
{
	m_renderer = renderer;
}

DescManager& DescManager::startLayout()
{
	m_layoutBindingBuffer.clear();

	return *this;
}

DescManager& DescManager::addLayoutBinding(VkDescriptorType type, uint32_t binding, uint32_t count, VkShaderStageFlags stage)
{
	VkDescriptorSetLayoutBinding layoutBinding{};
	layoutBinding.binding = binding;
	layoutBinding.descriptorType = type;
	layoutBinding.descriptorCount = count;
	layoutBinding.stageFlags = stage;
	layoutBinding.pImmutableSamplers = nullptr;
	m_layoutBindingBuffer.push_back(layoutBinding);

	return *this;
}

void DescManager::buildLayout(const std::string& name)
{
	VkDescriptorSetLayoutCreateInfo layoutInfo{};
	layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
	layoutInfo.bindingCount = m_layoutBindingBuffer.size();
	layoutInfo.pBindings = m_layoutBindingBuffer.data();
 
	/* What happens if already in the map */
	DescriptorRessources& res = m_descriptorRessources[name]; // TODO

	if (vkCreateDescriptorSetLayout(m_renderer->m_device, &layoutInfo, nullptr, &res.setLayout) != VK_SUCCESS)
		throw std::runtime_error("Vulkan: failed to create descriptor set layout!");

	for (auto& bindingBuffer : m_layoutBindingBuffer)
	{
		VkDescriptorPoolSize& poolSize = m_descriptorTypeMap[bindingBuffer.descriptorType];
		poolSize.type = bindingBuffer.descriptorType;
		poolSize.descriptorCount += bindingBuffer.descriptorCount * m_renderer->MAX_FRAMES_IN_FLIGHT;
	}
}

DescManager& DescManager::startSets(const std::string& name)
{
	m_setCreationResBuffer = &m_descriptorRessources.at(name);
	std::vector<VkDescriptorSetLayout> layouts(m_renderer->MAX_FRAMES_IN_FLIGHT, m_setCreationResBuffer->setLayout);
	VkDescriptorSetAllocateInfo allocInfo{};
	allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
	allocInfo.descriptorPool = m_descriptorPool;
	allocInfo.descriptorSetCount = (uint32_t)m_renderer->MAX_FRAMES_IN_FLIGHT;
	allocInfo.pSetLayouts = layouts.data();

	m_setCreationResBuffer->sets.resize(m_renderer->MAX_FRAMES_IN_FLIGHT);
	if (vkAllocateDescriptorSets(m_renderer->m_device, &allocInfo, m_setCreationResBuffer->sets.data()) != VK_SUCCESS)
		throw std::runtime_error("Vulkan: failed to create descriptor sets!");

	m_setCreationBufferInfoBuffer.clear();
	m_setCreationPerFrameBufferInfoBuffer.clear();
	m_setCreationImageInfoBuffer.clear();

	return *this;
}

DescManager& DescManager::addBufferInfo(VkBuffer buffer, VkDeviceSize offset, VkDeviceSize range)
{
	if (!m_setCreationResBuffer)
		throw std::runtime_error("DescriptorManager: no selected set for creation. Use \"startSets\" first!");

	VkDescriptorBufferInfo bufferInfo{};
	bufferInfo.buffer = buffer;
	bufferInfo.offset = 0;
	bufferInfo.range = range;
	m_setCreationBufferInfoBuffer.push_back(bufferInfo);

	return *this;
}

DescManager& DescManager::addPerFrameBufferInfo(const std::vector<VkBuffer>& buffers, VkDeviceSize offset, VkDeviceSize range)
{
	if (!m_setCreationResBuffer)
		throw std::runtime_error("DescriptorManager: no selected set for creation. Use \"startSets\" first!");

	if (buffers.size() != m_renderer->MAX_FRAMES_IN_FLIGHT)
		throw std::runtime_error("DescriptorManager: the buffer does not match the number of MAX_FRAMES_IN_FLIGHT");
	
	for (size_t i = 0; i < m_renderer->MAX_FRAMES_IN_FLIGHT; i++)
	{
		VkDescriptorBufferInfo bufferInfo{};
		bufferInfo.buffer = buffers[i];
		bufferInfo.offset = 0;
		bufferInfo.range = range;
		m_setCreationPerFrameBufferInfoBuffer.push_back(bufferInfo);
	}
}

DescManager& DescManager::addImageInfo(VkImageView imageView, VkImageLayout imageLayout, VkSampler imageSampler)
{
	if (!m_setCreationResBuffer)
		throw std::runtime_error("DescriptorManager: no selected set for creation. Use \"startSets\" first!");

	VkDescriptorImageInfo imageInfo{};
	imageInfo.imageView = imageView;
	imageInfo.imageLayout = imageLayout;
	imageInfo.sampler = imageSampler;

	return *this;
}

/* Builds an amount of sets equal to MAX_FRAMES_IN_FLIGHT */
void DescManager::buildSets()
{
	if (!m_setCreationResBuffer)
		throw std::runtime_error("DescriptorManager: no selected set for creation. Use \"startSets\" first!");

	for (size_t i = 0; i < m_renderer->MAX_FRAMES_IN_FLIGHT; i++)
	{
		for(size_t j = )
	}
}

VkDescriptorSetLayout DescManager::getLayout(const std::string& name)
{
	/* Does this trow an error if nothing is found or does it crash? */
	return m_descriptorRessources.at(name).setLayout; // TODO
}

void DescManager::createDescriptorPool()
{
	/* This implementation feels bad. I think it is better to change all the infrastructure from maps to vectors and save the corresponding indices in the scene ressources of renderer. */
	uint32_t poolSizeCount = m_descriptorTypeMap.size();
	std::vector<VkDescriptorPoolSize> poolSizes;
	poolSizes.resize(poolSizeCount);
	for (auto it = m_descriptorTypeMap.begin(); it != m_descriptorTypeMap.end(); it++)
	{
		poolSizes.push_back(it->second);
	}

	VkDescriptorPoolCreateInfo poolInfo{};
	poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
	poolInfo.poolSizeCount = poolSizeCount;
	poolInfo.pPoolSizes = poolSizes.data();
	poolInfo.maxSets = (uint32_t)(m_descriptorRessources.size() * m_renderer->MAX_FRAMES_IN_FLIGHT);
	if (vkCreateDescriptorPool(m_renderer->m_device, &poolInfo, nullptr, &m_descriptorPool) != VK_SUCCESS)
		throw std::runtime_error("Vulkan: failed to create descriptor pool!");
}
