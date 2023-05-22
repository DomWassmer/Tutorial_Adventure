#include <stdexcept>

#include "DescManager.h"

#include "Renderer3D.h"

DescManager::DescManager(Renderer3D* renderer)
{
	m_renderer = renderer;
}

DescManager& DescManager::startLayout()
{
	m_layoutBindingBuffer.clear();
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

void DescManager::createDescriptorSets()
{
	for (auto it = m_descriptorRessources.begin(); it != m_descriptorRessources.end(); it++)
	{
		std::vector<VkDescriptorSetLayout> layouts(m_renderer->MAX_FRAMES_IN_FLIGHT, it->second.setLayout);
		VkDescriptorSetAllocateInfo allocInfo{};
		allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
		allocInfo.descriptorPool = m_descriptorPool;
		allocInfo.descriptorSetCount = (uint32_t)m_renderer->MAX_FRAMES_IN_FLIGHT;
		allocInfo.pSetLayouts = layouts.data();

		it->second.sets.resize(m_renderer->MAX_FRAMES_IN_FLIGHT);
		if (vkAllocateDescriptorSets(m_renderer->m_device, &allocInfo, it->second.sets.data()) != VK_SUCCESS)
			throw std::runtime_error("Vulkan: failed to create descriptor sets!");

		for (size_t i = 0; i < m_renderer->MAX_FRAMES_IN_FLIGHT; i++)
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
}