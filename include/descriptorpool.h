#pragma once

#include "device.h"
#include <vulkan/vulkan_core.h>
class DescriptorPool {
private:
    VkDescriptorPool pool;
    Device* device;
public:
    DescriptorPool(Device* device, uint32_t count, VkDescriptorType type);
    ~DescriptorPool();
    VkDescriptorPool getHandle() { return pool; }
};