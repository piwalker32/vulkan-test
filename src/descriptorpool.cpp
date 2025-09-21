#include <descriptorpool.h>
#include <stdexcept>
#include <vulkan/vulkan_core.h>

DescriptorPool::DescriptorPool(Device* device, uint32_t count, VkDescriptorType type) :
        device(device) {
    VkDescriptorPoolSize poolSize{};
    poolSize.type = type;
    poolSize.descriptorCount = count;

    VkDescriptorPoolCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
    createInfo.poolSizeCount = 1;
    createInfo.pPoolSizes = &poolSize;
    createInfo.maxSets = count;

    if(vkCreateDescriptorPool(device->getDevice(), &createInfo, nullptr, &pool) != VK_SUCCESS) {
        throw std::runtime_error("FAILEC TO CREATE DESCRIPTOR SET POOL");
    }
}

DescriptorPool::~DescriptorPool() {
    vkDestroyDescriptorPool(device->getDevice(), pool, nullptr);
}