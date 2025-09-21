#pragma once

#include "device.h"
#include <vulkan/vulkan_core.h>
class Sampler {
private:
    VkSampler sampler;
    Device* device;
public:
    Sampler(Device* device, VkFilter filter = VK_FILTER_LINEAR, VkSamplerAddressMode repeat = VK_SAMPLER_ADDRESS_MODE_REPEAT);
    ~Sampler();
    VkSampler getHandle() { return sampler; }
};