#include <semaphore.h>
#include <stdexcept>
#include <vulkan/vulkan_core.h>

Semaphore::Semaphore(Device* device) : device(device) {
    VkSemaphoreCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

    if(vkCreateSemaphore(device->getDevice(), &createInfo, nullptr, &semaphore) != VK_SUCCESS) {
        throw std::runtime_error("FAILED TO CREATE SEMAPHORE");
    }
}

Semaphore::~Semaphore() {
    vkDestroySemaphore(device->getDevice(), semaphore, nullptr);
}