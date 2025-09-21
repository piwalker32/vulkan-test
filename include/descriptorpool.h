#pragma once

#include "device.h"
#include <vector>
#include <vulkan/vulkan_core.h>
class DescriptorPool {
private:
    VkDescriptorPool pool;
    Device* device;
public:
    DescriptorPool(Device* device, std::vector<uint32_t> counts, std::vector<VkDescriptorType> types);
    ~DescriptorPool();
    VkDescriptorPool getHandle() { return pool; }
};