#include "commandbuffer.h"
#include "commandpool.h"
#include "memory_util.h"
#include <buffer.h>
#include <stdexcept>
#include <vulkan/vulkan_core.h>

void Buffer::copyBuffer(Device* device, Buffer* src, Buffer* dst, VkDeviceSize size) {
    CommandPool pool(device, device->getQueueFamilies().graphicsFamily.value());
    CommandBuffer cmdBuffer(device, &pool);
    cmdBuffer.startRecording();
    VkBufferCopy copyRegion{};
    copyRegion.srcOffset = 0;
    copyRegion.dstOffset = 0;
    copyRegion.size = size;
    vkCmdCopyBuffer(cmdBuffer.getHandle(), src->getHandle(), dst->getHandle(), 1, &copyRegion);
    cmdBuffer.stopRecording();
    cmdBuffer.submit(device->getGraphicsQueue());
    vkQueueWaitIdle(device->getGraphicsQueue());
}

Buffer::Buffer(Device* device, uint64_t size, VkBufferUsageFlags usage, VkSharingMode sharingMode, VkMemoryPropertyFlags memoryProperties) :
    device(device), size(size) {
    VkBufferCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    createInfo.size = size;
    createInfo.usage = usage;
    createInfo.sharingMode = sharingMode;

    if(vkCreateBuffer(device->getDevice(), &createInfo, nullptr, &buffer) != VK_SUCCESS) {
        throw std::runtime_error("FAILED TO CREATE BUFFER");
    }

    VkMemoryRequirements memRequirements;
    vkGetBufferMemoryRequirements(device->getDevice(), buffer, &memRequirements);

    VkMemoryAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    allocInfo.allocationSize = memRequirements.size;
    allocInfo.memoryTypeIndex = findMemoryType(device, memRequirements.memoryTypeBits, memoryProperties);

    if(vkAllocateMemory(device->getDevice(), &allocInfo, nullptr, &bufferMemory) != VK_SUCCESS) {
        throw std::runtime_error("FAILED TO ALLOCATE BUFFER MEMORY");
    }

    vkBindBufferMemory(device->getDevice(), buffer, bufferMemory, 0);

}

Buffer::~Buffer() {
    vkDestroyBuffer(device->getDevice(), buffer, nullptr);
    vkFreeMemory(device->getDevice(), bufferMemory, nullptr);
}

void* Buffer::mapBuffer() {
    void* data;
    vkMapMemory(device->getDevice(), bufferMemory, 0, size, 0, &data);
    return data;
}

void Buffer::unmapBuffer() {
    vkUnmapMemory(device->getDevice(), bufferMemory);
}

void Buffer::bindVertex(CommandBuffer* cmdBuffer) {
    VkDeviceSize offsets[] = {0};
    vkCmdBindVertexBuffers(cmdBuffer->getHandle(), 0, 1, &buffer, offsets);
}

void Buffer::bindIndex(CommandBuffer* cmdBuffer) {
    vkCmdBindIndexBuffer(cmdBuffer->getHandle(), buffer, 0, VK_INDEX_TYPE_UINT16);
}