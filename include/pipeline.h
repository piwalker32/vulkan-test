#pragma once

#include "commandbuffer.h"
#include "device.h"
#include "swapchain.h"
#include <vector>
#include <vulkan/vulkan_core.h>
class Pipeline {
private:
    VkPipeline pipeline;
    Device* device;
    VkPipelineLayout layout;
    VkRenderPass renderPass;
public:
    Pipeline(Device* device, const std::vector<const char*> shaderFiles, SwapChain* swapchain, VkRenderPass renderPass);
    ~Pipeline();
    void bind(CommandBuffer* buffer);
};