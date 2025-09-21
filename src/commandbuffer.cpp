#include "fence.h"
#include <commandbuffer.h>
#include <cstdint>
#include <semaphore.h>
#include <stdexcept>
#include <vector>
#include <vulkan/vulkan_core.h>

CommandBuffer::CommandBuffer(Device* device, CommandPool* pool) :
    device(device) {
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

void CommandBuffer::reset() {
    vkResetCommandBuffer(buffer, 0);
}

void CommandBuffer::submit(VkQueue queue, Fence* fence, std::vector<Semaphore*> signalSemaphores, std::vector<Semaphore*> waitSemaphores, std::vector<VkPipelineStageFlags> waitStages) {
    VkSubmitInfo submitInfo{};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    std::vector<VkSemaphore> waitSemaphoreHandles;
    for(auto semaphore : waitSemaphores) {
        waitSemaphoreHandles.push_back(semaphore->getHandle());
    }
    submitInfo.waitSemaphoreCount = static_cast<uint32_t>(waitSemaphores.size());
    submitInfo.pWaitSemaphores = waitSemaphoreHandles.data();
    submitInfo.pWaitDstStageMask = waitStages.data();

    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &buffer;

    std::vector<VkSemaphore> signalSemaphoreHandles;
    for(auto semaphore : signalSemaphores) {
        signalSemaphoreHandles.push_back(semaphore->getHandle());
    }
    submitInfo.signalSemaphoreCount = static_cast<uint32_t>(signalSemaphores.size());
    submitInfo.pSignalSemaphores = signalSemaphoreHandles.data();

    if(vkQueueSubmit(device->getGraphicsQueue(), 1, &submitInfo, fence != nullptr ? fence->getHandle() : VK_NULL_HANDLE) != VK_SUCCESS) {
        std::runtime_error("FAILED TO SUBMIT COMMAND BUFFER");
    }

}