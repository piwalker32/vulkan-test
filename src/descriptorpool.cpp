#include <descriptorpool.h>
#include <stdexcept>
#include <vector>
#include <vulkan/vulkan_core.h>

DescriptorPool::DescriptorPool(Device* device, std::vector<uint32_t> counts, std::vector<VkDescriptorType> types) :
        device(device) {
    std::vector<VkDescriptorPoolSize> poolSizes(types.size());
    uint32_t maxCount = 0;
    for(uint32_t i = 0; i < types.size(); i++){
        VkDescriptorPoolSize poolSize{};
        poolSize.type = types[i];
        poolSize.descriptorCount = counts[i];
        poolSizes[i] = poolSize;
        if(counts[i] > maxCount)
            maxCount = counts[i];
    }

    VkDescriptorPoolCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
    createInfo.poolSizeCount = static_cast<uint32_t>(poolSizes.size());
    createInfo.pPoolSizes = poolSizes.data();
    createInfo.maxSets = maxCount;

    if(vkCreateDescriptorPool(device->getDevice(), &createInfo, nullptr, &pool) != VK_SUCCESS) {
        throw std::runtime_error("FAILEC TO CREATE DESCRIPTOR SET POOL");
    }
}

DescriptorPool::~DescriptorPool() {
    vkDestroyDescriptorPool(device->getDevice(), pool, nullptr);
}