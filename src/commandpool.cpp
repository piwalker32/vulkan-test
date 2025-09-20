#include <commandpool.h>
#include <stdexcept>
#include <vulkan/vulkan_core.h>

CommandPool::CommandPool(Device* device, uint32_t queueFamilyIndex) :
    device(device) {
    VkCommandPoolCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    createInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
    createInfo.queueFamilyIndex = queueFamilyIndex;

    if(vkCreateCommandPool(device->getDevice(), &createInfo, nullptr, &pool) != VK_SUCCESS) {
        throw std::runtime_error("FAILED TO CREATE COMMAND POOL");
    }
}

CommandPool::~CommandPool() {
    vkDestroyCommandPool(device->getDevice(), pool, nullptr);
}