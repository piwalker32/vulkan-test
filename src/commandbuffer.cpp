#include <commandbuffer.h>
#include <stdexcept>
#include <vulkan/vulkan_core.h>

CommandBuffer::CommandBuffer(Device* device, CommandPool* pool) {
    VkCommandBufferAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    allocInfo.commandPool = pool->getHandle();
    allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    allocInfo.commandBufferCount = 1;

    if (vkAllocateCommandBuffers(device->getDevice(), &allocInfo, &buffer) != VK_SUCCESS) {
        throw std::runtime_error("FAILED TO ALLOCATE COMMAND BUFFER");
    }
}

void CommandBuffer::startRecording() {
    VkCommandBufferBeginInfo beginInfo{};
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    beginInfo.flags = 0;
    beginInfo.pInheritanceInfo = nullptr;

    if(vkBeginCommandBuffer(buffer, &beginInfo) != VK_SUCCESS) {
        throw std::runtime_error("FAILED TO BEGIN RECORDING COMMAND BUFFER");
    }
}

void CommandBuffer::stopRecording() {
    if(vkEndCommandBuffer(buffer) != VK_SUCCESS) {
        throw std::runtime_error("FAILED RECORD COMMAND BUFFER");
    }
}