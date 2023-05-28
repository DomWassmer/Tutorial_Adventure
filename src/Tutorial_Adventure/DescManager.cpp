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
	layoutInfo.bindingCount = (uint32_t)m_layoutBindingBuffer.size();
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

	return *this;
}

DescManager& DescManager::addImageInfo(VkImageView imageView, VkImageLayout imageLayout, VkSampler imageSampler)
{
	if (!m_setCreationResBuffer)
		throw std::runtime_error("DescriptorManager: no selected set for creation. Use \"startSets\" first!");

	VkDescriptorImageInfo imageInfo{};
	imageInfo.imageView = imageView;
	imageInfo.imageLayout = imageLayout;
	imageInfo.sampler = imageSampler;
	m_setCreationImageInfoBuffer.push_back(imageInfo);

	return *this;
}

/* Builds an amount of sets equal to MAX_FRAMES_IN_FLIGHT */
void DescManager::buildSets()
{
	if (!m_setCreationResBuffer)
		throw std::runtime_error("DescriptorManager: no selected set for creation. Use \"startSets\" first!");

	for (size_t i = 0; i < m_renderer->MAX_FRAMES_IN_FLIGHT; i++)
	{
		std::vector<VkWriteDescriptorSet> descriptorWrites;
		
		for (size_t j = 0; j < m_setCreationBufferInfoBuffer.size(); j++)
		{
			VkWriteDescriptorSet descriptorWrite{};
			descriptorWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
			descriptorWrite.dstSet = m_setCreationResBuffer->sets[i];
			// Add how binding can be handled
			descriptorWrite.dstBinding = 0; // TODO
			descriptorWrite.dstArrayElement = 0;
			descriptorWrite.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
			descriptorWrite.descriptorCount = 1;
			descriptorWrite.pBufferInfo = &m_setCreationBufferInfoBuffer[j];

			descriptorWrites.push_back(descriptorWrite);
		}

		for (size_t j = 0; j < m_setCreationImageInfoBuffer.size(); j++)
		{
			VkWriteDescriptorSet descriptorWrite{};
			descriptorWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
			auto tmp = m_setCreationResBuffer->sets[i];
			descriptorWrite.dstSet = m_setCreationResBuffer->sets[i];
			// Add how binding can be handled
			descriptorWrite.dstBinding = 0; // TODO
			descriptorWrite.dstArrayElement = 0;
			descriptorWrite.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
			descriptorWrite.descriptorCount = 1;
			descriptorWrite.pImageInfo = &m_setCreationImageInfoBuffer[j];

			descriptorWrites.push_back(descriptorWrite);
		}

		for (size_t j = 0; j < (size_t)(m_setCreationPerFrameBufferInfoBuffer.size() 
			/ m_renderer->MAX_FRAMES_IN_FLIGHT); j++)
		{
			VkWriteDescriptorSet descriptorWrite{};
			descriptorWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
			descriptorWrite.dstSet = m_setCreationResBuffer->sets[i];
			// Add how binding can be handled
			descriptorWrite.dstBinding = 0; // TODO
			descriptorWrite.dstArrayElement = 0;
			descriptorWrite.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
			descriptorWrite.descriptorCount = 1;
			descriptorWrite.pBufferInfo = &m_setCreationPerFrameBufferInfoBuffer[j * m_renderer->MAX_FRAMES_IN_FLIGHT + i];

			descriptorWrites.push_back(descriptorWrite);
		}

		vkUpdateDescriptorSets(m_renderer->m_device, (uint32_t)descriptorWrites.size(),
			descriptorWrites.data(), 0, nullptr);
	}

	m_setCreationResBuffer = nullptr;
}

VkDescriptorSetLayout DescManager::getLayout(const std::string& name)
{
	/* Does this trow an error if nothing is found or does it crash? */
	return m_descriptorRessources.at(name).setLayout; // TODO
}

VkDescriptorSet DescManager::getDescriptorSet(const std::string& name, int frame)
{
	return m_descriptorRessources.at(name).sets[frame];
}

void DescManager::createDescriptorPool()
{
	/* This implementation feels bad. I think it is better to change all the infrastructure from maps to vectors and save the corresponding indices in the scene ressources of renderer. */
	uint32_t poolSizeCount = (uint32_t)m_descriptorTypeMap.size();
	std::vector<VkDescriptorPoolSize> poolSizes{};
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

void DescManager::cleanup()
{
	vkDestroyDescriptorPool(m_renderer->m_device, m_descriptorPool, nullptr);
	for (auto it = m_descriptorRessources.begin(); it != m_descriptorRessources.end(); it++)
	{
		vkDestroyDescriptorSetLayout(m_renderer->m_device, it->second.setLayout, nullptr);
	}
}