#pragma once

#include "device.h"
#include "image.h"
#include "imageview.h"
#include <memory>
#include <vulkan/vulkan_core.h>
class Texture {
private:
    Device* device;
    std::unique_ptr<Image> textureImage;
    std::unique_ptr<ImageView> textureImageView;
    VkSampler sampler;
public:
    Texture(Device* device, const char* file, VkFilter filter = VK_FILTER_LINEAR, VkSamplerAddressMode repeatMode = VK_SAMPLER_ADDRESS_MODE_REPEAT);
    ~Texture();
    VkSampler getSampler() { return sampler; }
    VkImageView getImageView() { return textureImageView->getImageView(); }
};