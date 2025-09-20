#include <imageview.h>
#include <iostream>
#include <stdexcept>
#include <vulkan/vulkan_core.h>

ImageView::ImageView(Device* device, VkImage image, VkFormat format, VkImageAspectFlagBits aspects, VkImageViewType type) :
    device(device) {
    VkImageViewCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
    createInfo.image = image;
    createInfo.viewType = type;
    createInfo.format = format;

    createInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
    createInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
    createInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
    createInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;

    createInfo.subresourceRange.aspectMask = aspects;
    createInfo.subresourceRange.baseMipLevel = 0;
    createInfo.subresourceRange.levelCount = 1;
    createInfo.subresourceRange.baseArrayLayer = 0;
    createInfo.subresourceRange.layerCount = 1;

    if(vkCreateImageView(device->getDevice(), &createInfo, nullptr, &imageview) != VK_SUCCESS) {
        throw std::runtime_error("FAILED TO CREATE IMAGEVIEW!");
    }
}

ImageView::~ImageView() {
    vkDestroyImageView(device->getDevice(), imageview, nullptr);
}