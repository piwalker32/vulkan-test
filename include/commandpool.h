#pragma once

#include "device.h"
#include <vulkan/vulkan_core.h>
class CommandPool {
private:
    VkCommandPool pool;
    Device* device;
public:
    CommandPool(Device* device, uint32_t queueFamilyIndex);
    ~CommandPool();
    VkCommandPool getHandle() { return pool; }
};