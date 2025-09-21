#pragma once

#include "buffer.h"
#include "commandpool.h"
#include "device.h"
#include <vulkan/vulkan_core.h>
class Image {
private:
    VkImage image;
    VkDeviceMemory imageMemory;
    VkFormat currentFormat;
    VkImageLayout currentLayout;
    Device* device;
    CommandPool cmdPool;
public:
    Image(Device* device, uint32_t width, uint32_t height, VkFormat format, VkImageTiling tiling, VkImageUsageFlags usage, VkMemoryPropertyFlags properties);
    ~Image();
    void transitionImageLayout(VkImageLayout newLayout);
    void bufferToImage(Buffer* buffer, uint32_t width, uint32_t height);
    VkImage getHandle() { return image; }
};