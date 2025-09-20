#pragma once

#include "device.h"
#include <vector>
#include <vulkan/vulkan_core.h>
class Pipeline {
private:
    VkPipeline pipeline;
    Device* device;
public:
    Pipeline(Device* device, const std::vector<const char*> shaderFiles);
    ~Pipeline();
};