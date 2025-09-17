#pragma once

#include <vulkan/vulkan_core.h>
#include <instance.h>

class Device {
private:
    VkPhysicalDevice physicalDevice = VK_NULL_HANDLE;
public:
    Device(Instance* instance);
    ~Device();
};