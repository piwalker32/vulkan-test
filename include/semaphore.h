#pragma once

#include "device.h"
#include <vulkan/vulkan_core.h>
class Semaphore {
private:
    VkSemaphore semaphore;
    Device* device;
public:
    Semaphore(Device* device);
    ~Semaphore();
    VkSemaphore getHandle() { return semaphore; }
};