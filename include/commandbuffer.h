#pragma once

#include "commandpool.h"
#include "device.h"
#include "fence.h"
#include <semaphore.h>
#include <vector>
#include <vulkan/vulkan_core.h>
class CommandBuffer {
private:
    VkCommandBuffer buffer;
    CommandPool* pool;
    Device* device;
public:
    CommandBuffer(Device* device, CommandPool* pool);
    ~CommandBuffer();
    void startRecording();
    void stopRecording();
    void reset();
    void submit(VkQueue queue, Fence* fence = nullptr, std::vector<Semaphore*> signalSemaphores = {}, std::vector<Semaphore*> waitSemaphores = {}, std::vector<VkPipelineStageFlags> waitStages = {});
    VkCommandBuffer getHandle() { return buffer; }
};