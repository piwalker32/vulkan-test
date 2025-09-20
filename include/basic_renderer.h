#pragma once

#include "commandbuffer.h"
#include "commandpool.h"
#include "device.h"
#include "framebuffer.h"
#include "pipeline.h"
#include "swapchain.h"
#include <vector>
#include <vulkan/vulkan_core.h>
class BasicRenderer {
private:
    Device* device;
    SwapChain* swapchain;
    VkRenderPass renderPass;
    Pipeline pipeline;
    std::vector<Framebuffer> framebuffers;
    CommandPool pool;
    std::vector<CommandBuffer> buffers;
public:
    BasicRenderer(Device* device, SwapChain* swapchain);
    ~BasicRenderer();
    void render(size_t frame, Fence* fence, std::vector<Semaphore*> signalSemaphores = {}, std::vector<Semaphore*> waitSemaphores = {}, std::vector<VkPipelineStageFlags> waitStages = {});
private:
    void beginRenderPass(size_t frame);
};