#pragma once

#include "device.h"
#include <string>
#include <vulkan/vulkan_core.h>
class Shader {
private:
    VkShaderModule shaderModule;
    Device* device;
public:
    Shader(Device* device, const std::string& filename);
    ~Shader();
    VkShaderModule getShader() { return shaderModule; }
};