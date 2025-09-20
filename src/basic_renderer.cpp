#include "commandbuffer.h"
#include "imageview.h"
#include "pipeline.h"
#include <basic_renderer.h>
#include <stdexcept>
#include <vector>
#include <vulkan/vulkan_core.h>

const std::vector<const char*> shaders = {
    "basic.vert.spv",
    "basic.frag.spv"
};

static VkRenderPass createRenderPass(Device* device, SwapChain* swapchain) {
    //renderpass attachments, defines all framebuffers that could be attached may need further development in future
    VkAttachmentDescription colorAttachment{};
    colorAttachment.format = swapchain->getSwapChainFormat();
    colorAttachment.samples = VK_SAMPLE_COUNT_1_BIT;

    colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
    colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;

    colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    colorAttachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

    VkAttachmentReference colorAttachmentRef{};
    colorAttachmentRef.attachment = 0;
    colorAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

    //define subpasses, for now we only have one, but we may want to perform multiple passes in the future for various post processing effects
    VkSubpassDescription subpass{};
    subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
    subpass.colorAttachmentCount = 1;
    subpass.pColorAttachments = &colorAttachmentRef;

    VkRenderPassCreateInfo renderPassInfo{};
    renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
    renderPassInfo.attachmentCount = 1;
    renderPassInfo.pAttachments = &colorAttachment;
    renderPassInfo.subpassCount = 1;
    renderPassInfo.pSubpasses = &subpass;

    VkRenderPass renderPass;

    if(vkCreateRenderPass(device->getDevice(), &renderPassInfo, nullptr, &renderPass) != VK_SUCCESS) {
        throw std::runtime_error("FAILED TO CREATE RENDER PASS");
    }

    return renderPass;
}

BasicRenderer::BasicRenderer(Device* device, SwapChain* swapchain)
    :device(device), swapchain(swapchain), renderPass(createRenderPass(device, swapchain)), pipeline(device, shaders, swapchain, renderPass),
    pool(device, device->getQueueFamilies().graphicsFamily.value()), buffer(device, &pool) {
    
    framebuffers.reserve(swapchain->getImageCount());
    for(size_t i = 0; i < swapchain->getImageCount(); i++) {
        framebuffers.emplace_back(device, renderPass, swapchain->getSwapChainExtent(), std::vector<VkImageView>{ swapchain->getImageView(i)->getImageView() });
    }
}

BasicRenderer::~BasicRenderer() {
    vkDestroyRenderPass(device->getDevice(), renderPass, nullptr);
}

void BasicRenderer::beginRenderPass() {
    uint32_t frameIndex = 0; //TODO: get current frame from swapchain
    VkRenderPassBeginInfo renderPassInfo{};
    renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    renderPassInfo.renderPass = renderPass;
    renderPassInfo.framebuffer = framebuffers[frameIndex].gethandle();
    renderPassInfo.renderArea.offset = {0, 0};
    renderPassInfo.renderArea.extent = swapchain->getSwapChainExtent();

    VkClearValue clearColor = {{{0.0f, 0.0f, 0.0f, 1.0f}}};
    renderPassInfo.clearValueCount = 1;
    renderPassInfo.pClearValues = &clearColor;
    vkCmdBeginRenderPass(buffer.getHandle(), &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);
}

void BasicRenderer::render() {
    buffer.startRecording();
    beginRenderPass();
    pipeline.bind(&buffer);
    //TODO specify dynamic viewport and scissor state
    vkCmdDraw(buffer.getHandle(), 3, 1, 0, 0);
    vkCmdEndRenderPass(buffer.getHandle());
    
}