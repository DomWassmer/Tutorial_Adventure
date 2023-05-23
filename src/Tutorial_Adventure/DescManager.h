#include <unordered_map>
#include <vector>
#include <string>
#include <memory>

#include <vulkan/vulkan.h>

class Renderer3D;

/* Only to be called from its parent renderer */
class DescManager {
public:
	DescManager(Renderer3D* renderer);

	DescManager(const DescManager&) = delete;
	DescManager& operator=(const DescManager&) = delete;

	/* Builder functions for creating descriptor set layouts */
	DescManager& startLayout();
	DescManager& addLayoutBinding(VkDescriptorType type, uint32_t binding, uint32_t count, VkShaderStageFlags stage);
	void buildLayout(const std::string& name);

	/* Builder functions for creating descriptors sets */
	DescManager& startSets(const std::string& name);
	DescManager& addBufferInfo(VkBuffer buffer, VkDeviceSize offset, VkDeviceSize range);
	DescManager& addPerFrameBufferInfo(const std::vector<VkBuffer>& buffers, VkDeviceSize offset, VkDeviceSize range);
	DescManager& addImageInfo(VkImageView imageView, VkImageLayout imageLayout, VkSampler imageSampler);
	void buildSets();

	VkDescriptorSetLayout getLayout(const std::string& name);

	void createDescriptorPool();
	void cleanup();
private:
	struct DescriptorRessources {
		VkDescriptorSetLayout setLayout;
		std::vector<VkDescriptorSet> sets;
	};

	/* DescManager should only be called from this renderer here.
	Then the pointer is never nullptr */
	Renderer3D* m_renderer;

	/* Buffers for Set Layout Creation */
	std::vector<VkDescriptorSetLayoutBinding> m_layoutBindingBuffer;

	/* Buffers for Set Creation */
	DescriptorRessources* m_setCreationResBuffer = nullptr;
	std::vector<VkDescriptorBufferInfo> m_setCreationBufferInfoBuffer;
	std::vector<VkDescriptorBufferInfo> m_setCreationPerFrameBufferInfoBuffer;
	std::vector<VkDescriptorImageInfo> m_setCreationImageInfoBuffer;

	std::unordered_map<VkDescriptorType, VkDescriptorPoolSize> m_descriptorTypeMap;
	std::unordered_map<std::string, DescriptorRessources> m_descriptorRessources;
	VkDescriptorPool m_descriptorPool;
};