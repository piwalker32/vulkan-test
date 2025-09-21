#pragma once

#include "commandbuffer.h"
#include "device.h"
#include <cstdint>
#include <vulkan/vulkan_core.h>
class Buffer {
private:
    VkBuffer buffer;
    VkDeviceMemory bufferMemory;
    Device* device;
    uint64_t size;
public:
    Buffer(Device* device, uint64_t size, VkBufferUsageFlags usage, VkSharingMode sharingMode, VkMemoryPropertyFlags memoryProperties);
    ~Buffer();
    void* mapBuffer();
    void unmapBuffer();
    void bindVertex(CommandBuffer* cmdBuffer);
    void bindIndex(CommandBuffer* cmdBuffer);
    VkBuffer getHandle() { return buffer; }
    uint64_t getSize() { return size; }

    static void copyBuffer(Device* device, Buffer* src, Buffer* dst, VkDeviceSize size);
};