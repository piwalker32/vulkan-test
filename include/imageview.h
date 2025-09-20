#pragma once

#include "device.h"
#include <vulkan/vulkan.h>
#include <vulkan/vulkan_core.h>

class ImageView {
private:
    VkImageView imageview = VK_NULL_HANDLE;
    Device* device;
public:
    ImageView(Device* device, VkImage image, VkFormat format, VkImageAspectFlagBits aspects, VkImageViewType type = VK_IMAGE_VIEW_TYPE_2D);
    ~ImageView();
    VkImageView getImageView() { return imageview; }
};