#include "commandbuffer.h"
#include <buffer.h>
#include <stdexcept>
#include <vulkan/vulkan_core.h>

uint32_t findMemoryType(Device* device, uint32_t typeFilter, VkMemoryPropertyFlags properties) {
    VkPhysicalDeviceMemoryProperties memProperties;
    vkGetPhysicalDeviceMemoryProperties(device->getPhysicalDevices(), &memProperties);

    for(uint32_t i = 0; i < memProperties.memoryTypeCount; i++) {
        if((typeFilter & (1 << i)) && (memProperties.memoryTypes[i].propertyFlags & properties) == properties) {
            return i;
        }
    }

    throw std::runtime_error("FAILED TO FIND SUITABLE MEMORY TYPE FOR BUFFER");
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