#pragma once

#include "surface.h"
#include <vulkan/vulkan_core.h>
#include <instance.h>

class Device {
private:
    VkPhysicalDevice physicalDevice = VK_NULL_HANDLE;
    VkDevice device;
    VkQueue graphicsQueue;
    VkQueue presentQueue = VK_NULL_HANDLE;
public:
    Device(Instance* instance, Surface* surface = nullptr);
    ~Device();
};