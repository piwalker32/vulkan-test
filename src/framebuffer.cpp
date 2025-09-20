#include <framebuffer.h>
#include <stdexcept>
#include <vulkan/vulkan_core.h>

Framebuffer::Framebuffer(Device* device, VkRenderPass renderPass, VkExtent2D extent, std::vector<VkImageView> attachments) :
    device(device) {
    VkFramebufferCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
    createInfo.renderPass = renderPass;
    createInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
    createInfo.pAttachments = attachments.data();
    createInfo.width = extent.width;
    createInfo.height = extent.height;
    createInfo.layers = 1;

    if(vkCreateFramebuffer(device->getDevice(), &createInfo, nullptr, &framebuffer) != VK_SUCCESS) {
        throw std::runtime_error("FAILED TO CREATE FRAMEBUFFER");
    }
}

Framebuffer::~Framebuffer() {
    vkDestroyFramebuffer(device->getDevice(), framebuffer, nullptr);
}