#include <cstdint>
#include <fence.h>
#include <stdexcept>
#include <vulkan/vulkan_core.h>

Fence::Fence(Device* device, bool signaled) : device(device) {
    VkFenceCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
    if(signaled)
        createInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

    if(vkCreateFence(device->getDevice(), &createInfo, nullptr, &fence) != VK_SUCCESS) {
        throw std::runtime_error("FAILED TO CREATE FENCE");
    }
}

Fence::~Fence() {
    vkDestroyFence(device->getDevice(), fence, nullptr);
}

void Fence::wait() {
    vkWaitForFences(device->getDevice(), 1, &fence, VK_TRUE, UINT64_MAX);
}

void Fence::reset() {
    vkResetFences(device->getDevice(), 1, &fence);
}