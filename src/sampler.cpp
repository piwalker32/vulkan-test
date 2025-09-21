#include <sampler.h>
#include <stdexcept>

Sampler::Sampler(Device* device, VkFilter filter, VkSamplerAddressMode repeat) :
    device(device) {
    VkSamplerCreateInfo samplerInfo{};
    samplerInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
    samplerInfo.magFilter = filter;
    samplerInfo.minFilter = filter;

    samplerInfo.addressModeU = repeat;
    samplerInfo.addressModeV = repeat;
    samplerInfo.addressModeW = repeat;

    VkPhysicalDeviceProperties deviceProps{};
    vkGetPhysicalDeviceProperties(device->getPhysicalDevices(), &deviceProps);

    samplerInfo.anisotropyEnable = VK_TRUE;
    samplerInfo.maxAnisotropy = deviceProps.limits.maxSamplerAnisotropy;

    samplerInfo.borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK;
    samplerInfo.unnormalizedCoordinates = VK_FALSE;

    samplerInfo.compareEnable = VK_FALSE;
    samplerInfo.compareOp = VK_COMPARE_OP_ALWAYS;

    samplerInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
    samplerInfo.mipLodBias = 0.0f;
    samplerInfo.minLod = 0.0f;
    samplerInfo.maxLod = 0.0f;

    if(vkCreateSampler(device->getDevice(), &samplerInfo, nullptr, &sampler) != VK_SUCCESS) {
        throw std::runtime_error("FAILED TO CREATE TEXTURE SAMPLER");
    }
}

Sampler::~Sampler() {
    vkDestroySampler(device->getDevice(), sampler, nullptr);
}