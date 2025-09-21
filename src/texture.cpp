#include "buffer.h"
#include "image.h"
#include "imageview.h"
#include "memory_util.h"
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <memory>
#include <stdexcept>
#include <texture.h>
#include <vulkan/vulkan_core.h>
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

Texture::Texture(Device* device, const char* file) :
    device(device) {
    int texWidth, texHeight, texChannels;
    stbi_uc* pixels = stbi_load(file, &texWidth, &texHeight, &texChannels, STBI_rgb_alpha);
    VkDeviceSize imageSize = texWidth * texHeight * 4;

    textureImage = std::unique_ptr<Image>(new Image(device, texWidth, texHeight, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT));

    if(!pixels) {
        throw std::runtime_error("FAILED TO LOAD TEXTURE IMAGE");
    }

    Buffer stagingBuffer(device, imageSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_SHARING_MODE_EXCLUSIVE, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
    void* data = stagingBuffer.mapBuffer();
    memcpy(data, pixels, static_cast<size_t>(imageSize));
    stagingBuffer.unmapBuffer();
    stbi_image_free(pixels);
    
    textureImage->transitionImageLayout(VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);
    textureImage->bufferToImage(&stagingBuffer, texWidth, texHeight);
    textureImage->transitionImageLayout(VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);

    textureImageView = std::unique_ptr<ImageView>(new ImageView(device, textureImage->getHandle(), VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_ASPECT_COLOR_BIT));

}

Texture::~Texture() {
    
}