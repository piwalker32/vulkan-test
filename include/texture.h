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
public:
    Texture(Device* device, const char* file);
    ~Texture();
    VkImageView getImageView() { return textureImageView->getImageView(); }
};