#include "commandbuffer.h"
#include "memory_util.h"
#include <image.h>
#include <stdexcept>
#include <vulkan/vulkan_core.h>

Image::Image(Device* device, uint32_t width, uint32_t height, VkFormat format, VkImageTiling tiling, VkImageUsageFlags usage, VkMemoryPropertyFlags properties) :
    device(device), currentFormat(format), currentLayout(VK_IMAGE_LAYOUT_UNDEFINED), cmdPool(device, device->getQueueFamilies().graphicsFamily.value()) {
    
    VkImageCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
    createInfo.imageType = VK_IMAGE_TYPE_2D;
    createInfo.extent.width = static_cast<uint32_t>(width);
    createInfo.extent.height = static_cast<uint32_t>(height);
    createInfo.extent.depth = 1;
    createInfo.mipLevels = 1;
    createInfo.arrayLayers = 1;
    createInfo.format = VK_FORMAT_R8G8B8A8_SRGB;
    createInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
    createInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    createInfo.usage = VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;
    createInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
    createInfo.samples = VK_SAMPLE_COUNT_1_BIT;
    createInfo.flags = 0;

    if(vkCreateImage(device->getDevice(), &createInfo, nullptr, &image) != VK_SUCCESS) {
        throw std::runtime_error("FAILED TO CREATE TEXTURE IMAGE");
    }

    VkMemoryRequirements memRequirements;
    vkGetImageMemoryRequirements(device->getDevice(), image, &memRequirements);

    VkMemoryAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    allocInfo.allocationSize = memRequirements.size;
    allocInfo.memoryTypeIndex = findMemoryType(device, memRequirements.memoryTypeBits, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

    if(vkAllocateMemory(device->getDevice(), &allocInfo, nullptr, &imageMemory) != VK_SUCCESS) {
        throw std::runtime_error("FAILED TO ALLOCATE TEXTURE MEMORY");
    }

    vkBindImageMemory(device->getDevice(), image, imageMemory, 0);
}

Image::~Image() {
    vkDestroyImage(device->getDevice(), image, nullptr);
    vkFreeMemory(device->getDevice(), imageMemory, nullptr);
}

void Image::transitionImageLayout(VkImageLayout newLayout) {
    CommandBuffer cmdBuffer(device, &cmdPool);
    cmdBuffer.startRecording();

    VkImageMemoryBarrier barrier{};
    barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
    barrier.oldLayout = currentLayout;
    barrier.newLayout = newLayout;

    barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;

    barrier.image = image;
    barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    barrier.subresourceRange.baseMipLevel = 0;
    barrier.subresourceRange.levelCount = 1;
    barrier.subresourceRange.baseArrayLayer = 0;
    barrier.subresourceRange.layerCount = 1;

    barrier.srcAccessMask = 0; //TODO
    barrier.dstAccessMask = 0; //TODO

    VkPipelineStageFlags sourceStage;
    VkPipelineStageFlags destinationStage;

    if(currentLayout == VK_IMAGE_LAYOUT_UNDEFINED && newLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL) {
        barrier.srcAccessMask = 0;
        barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;

        sourceStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
        destinationStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
    } else if(currentLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL && newLayout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL) {
        barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
        barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

        sourceStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
        destinationStage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
    } else {
        throw std::runtime_error("UNSUPPORTED LAYOUT TRANSITION");
    }

    vkCmdPipelineBarrier(cmdBuffer.getHandle(), sourceStage, destinationStage, 0, 0, nullptr, 0, nullptr, 1, &barrier);

    cmdBuffer.stopRecording();
    cmdBuffer.submit(device->getGraphicsQueue());
    vkQueueWaitIdle(device->getGraphicsQueue());
    currentLayout = newLayout;
}

void Image::bufferToImage(Buffer* buffer, uint32_t width, uint32_t height) {
    CommandBuffer cmdBuffer(device, &cmdPool);
    cmdBuffer.startRecording();

    VkBufferImageCopy region{};
    region.bufferOffset = 0;
    region.bufferRowLength = 0;
    region.bufferImageHeight = 0;

    region.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    region.imageSubresource.mipLevel = 0;
    region.imageSubresource.baseArrayLayer = 0;
    region.imageSubresource.layerCount = 1;

    region.imageOffset = {0, 0, 0};
    region.imageExtent = {width, height, 1};

    vkCmdCopyBufferToImage(cmdBuffer.getHandle(), buffer->getHandle(), image, currentLayout, 1, &region);

    cmdBuffer.stopRecording();
    cmdBuffer.submit(device->getGraphicsQueue());
    vkQueueWaitIdle(device->getGraphicsQueue());
}