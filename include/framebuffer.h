#pragma once

#include "device.h"
#include <vector>
#include <vulkan/vulkan_core.h>
class Framebuffer {
private:
    VkFramebuffer framebuffer;
    Device* device;
public:
    Framebuffer(Device* device, VkRenderPass renderPass, VkExtent2D extent, std::vector<VkImageView> attachments);
    ~Framebuffer();
    VkFramebuffer gethandle() { return framebuffer; }
};