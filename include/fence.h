#pragma once

#include "device.h"
#include <vulkan/vulkan_core.h>
class Fence {
private:
    VkFence fence;
    Device* device;
public:
    Fence(Device* device, bool signaled = false);
    ~Fence();
    void wait();
    void reset();
    VkFence getHandle() { return fence; }
};