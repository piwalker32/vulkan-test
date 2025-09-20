#pragma once

#include "commandpool.h"
#include <vulkan/vulkan_core.h>
class CommandBuffer {
private:
    VkCommandBuffer buffer;
public:
    CommandBuffer(Device* device, CommandPool* pool);
    void startRecording();
    void stopRecording();
    VkCommandBuffer getHandle() { return buffer; }
};