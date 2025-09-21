#include "descriptorpool.h"
#include "image.h"
#include "swapchain.h"
#include "texture.h"
#include <array>
#include <chrono>
#include <glm/ext/matrix_clip_space.hpp>
#include <glm/ext/matrix_transform.hpp>
#include <glm/ext/vector_float3.hpp>
#include <glm/trigonometric.hpp>
#define GLM_FORCE_RADIANS
#include "buffer.h"
#include "commandbuffer.h"
#include "global_config.h"
#include "imageview.h"
#include "pipeline.h"
#include <basic_renderer.h>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <ratio>
#include <stdexcept>
#include <vector>
#include <vulkan/vulkan_core.h>
#include <glm/gtc/matrix_transform.hpp>

const std::vector<const char*> shaders = {
    "basic.vert.spv",
    "basic.frag.spv"
};

const std::vector<Vertex> verticies = {
    {{-0.5f, -0.5f}, {1.0f, 0.0f, 0.0f}, {1.0f, 0.0f}},
    {{0.5f, -0.5f}, {0.0f, 1.0f, 0.0f}, {0.0f, 0.0f}},
    {{0.5f, 0.5f}, {0.0f, 0.0f, 1.0f}, {0.0f, 1.0f}},
    {{-0.5f, 0.5f}, {1.0f, 1.0f, 1.0f}, {1.0f, 1.0f}}
};

const std::vector<uint16_t> indicies = {
    0, 1, 2, 2, 3, 0
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

    VkSubpassDependency dependency{};
    dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
    dependency.dstSubpass = 0;
    dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    dependency.srcAccessMask = 0;
    dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

    VkRenderPassCreateInfo renderPassInfo{};
    renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
    renderPassInfo.attachmentCount = 1;
    renderPassInfo.pAttachments = &colorAttachment;
    renderPassInfo.subpassCount = 1;
    renderPassInfo.pSubpasses = &subpass;
    renderPassInfo.dependencyCount = 1;
    renderPassInfo.pDependencies = &dependency;

    VkRenderPass renderPass;

    if(vkCreateRenderPass(device->getDevice(), &renderPassInfo, nullptr, &renderPass) != VK_SUCCESS) {
        throw std::runtime_error("FAILED TO CREATE RENDER PASS");
    }

    return renderPass;
}

BasicRenderer::BasicRenderer(Device* device, SwapChain* swapchain)
    :device(device), swapchain(swapchain), renderPass(createRenderPass(device, swapchain)), pipeline(device, shaders, swapchain, renderPass),
    pool(device, device->getQueueFamilies().graphicsFamily.value()), 
    vertexBuffer(device, sizeof(Vertex) * verticies.size(), VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, VK_SHARING_MODE_EXCLUSIVE, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT),
    indexBuffer(device, sizeof(uint16_t) * indicies.size(), VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT, VK_SHARING_MODE_EXCLUSIVE, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT),
    descriptorPool(device, std::vector<uint32_t>(2, MAX_FRAMES_IN_FLIGHT), std::vector<VkDescriptorType>{VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER}),
    texture(device, "res/texture/statue.jpg") {
    
    createFramebuffers();

    std::vector<VkDescriptorSetLayout> layouts(MAX_FRAMES_IN_FLIGHT, pipeline.getDescriptorSetLayout());
    VkDescriptorSetAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
    allocInfo.descriptorPool = descriptorPool.getHandle();
    allocInfo.descriptorSetCount = static_cast<uint32_t>(MAX_FRAMES_IN_FLIGHT);
    allocInfo.pSetLayouts = layouts.data();

    descriptorSets.resize(MAX_FRAMES_IN_FLIGHT);
    if(vkAllocateDescriptorSets(device->getDevice(), &allocInfo, descriptorSets.data()) != VK_SUCCESS) {
        throw std::runtime_error("FAILED TO ALLOCATE DESCRIPTOR SETS");
    }

    buffers.reserve(MAX_FRAMES_IN_FLIGHT);
    uniformBuffers.reserve(MAX_FRAMES_IN_FLIGHT);
    uniformBuffersMapped.reserve(MAX_FRAMES_IN_FLIGHT);
    for(size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
        buffers.emplace_back(device, &pool);
        uniformBuffers.emplace_back(device, sizeof(UniformBufferObject), VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VK_SHARING_MODE_EXCLUSIVE, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
        uniformBuffersMapped.push_back(uniformBuffers[i].mapBuffer());

        VkDescriptorBufferInfo bufferInfo{};
        bufferInfo.buffer = uniformBuffers[i].getHandle();
        bufferInfo.offset = 0;
        bufferInfo.range = sizeof(UniformBufferObject);

        VkDescriptorImageInfo imageInfo{};
        imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
        imageInfo.imageView = texture.getImageView();
        imageInfo.sampler = texture.getSampler();

        std::array<VkWriteDescriptorSet, 2> descriptorWrites{};
        descriptorWrites[0].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        descriptorWrites[0].dstSet = descriptorSets[i];
        descriptorWrites[0].dstBinding = 0;
        descriptorWrites[0].dstArrayElement = 0;
        descriptorWrites[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        descriptorWrites[0].descriptorCount = 1;
        descriptorWrites[0].pBufferInfo = &bufferInfo;
        descriptorWrites[0].pImageInfo = nullptr;
        descriptorWrites[0].pTexelBufferView = nullptr;

        descriptorWrites[1].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        descriptorWrites[1].dstSet = descriptorSets[i];
        descriptorWrites[1].dstBinding = 1;
        descriptorWrites[1].dstArrayElement = 0;
        descriptorWrites[1].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
        descriptorWrites[1].descriptorCount = 1;
        descriptorWrites[1].pBufferInfo = nullptr;
        descriptorWrites[1].pImageInfo = &imageInfo;
        descriptorWrites[1].pTexelBufferView = nullptr;

        vkUpdateDescriptorSets(device->getDevice(), static_cast<uint32_t>(descriptorWrites.size()), descriptorWrites.data(), 0, nullptr);
    }

    {
        uint64_t size = sizeof(Vertex) * verticies.size();
        Buffer stagingBuffer(device, size, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_SHARING_MODE_EXCLUSIVE, 
            VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

        void* data = stagingBuffer.mapBuffer();
        memcpy(data, verticies.data(), size);
        stagingBuffer.unmapBuffer();

        Buffer::copyBuffer(device, &stagingBuffer, &vertexBuffer, size);

    }

    {
        uint64_t size = sizeof(uint16_t) * indicies.size();
        Buffer stagingBuffer(device, size, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_SHARING_MODE_EXCLUSIVE, 
            VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

        void* data = stagingBuffer.mapBuffer();
        memcpy(data, indicies.data(), size);
        stagingBuffer.unmapBuffer();

        Buffer::copyBuffer(device, &stagingBuffer, &indexBuffer, size);

    }

    
}

BasicRenderer::~BasicRenderer() {
    vkDestroyRenderPass(device->getDevice(), renderPass, nullptr);
}

void BasicRenderer::destroyFramebuffers() {
    framebuffers.clear();
}

void BasicRenderer::createFramebuffers() {
    framebuffers.reserve(swapchain->getImageCount());
    for(size_t i = 0; i < swapchain->getImageCount(); i++) {
        framebuffers.emplace_back(device, renderPass, swapchain->getSwapChainExtent(), std::vector<VkImageView>{ swapchain->getImageView(i)->getImageView() });
    }
}

void BasicRenderer::beginRenderPass(size_t frame) {
    uint32_t frameIndex = swapchain->getImageIndex();
    VkRenderPassBeginInfo renderPassInfo{};
    renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    renderPassInfo.renderPass = renderPass;
    renderPassInfo.framebuffer = framebuffers[frameIndex].gethandle();
    renderPassInfo.renderArea.offset = {0, 0};
    renderPassInfo.renderArea.extent = swapchain->getSwapChainExtent();

    VkClearValue clearColor = {{{0.0f, 0.0f, 0.0f, 1.0f}}};
    renderPassInfo.clearValueCount = 1;
    renderPassInfo.pClearValues = &clearColor;
    vkCmdBeginRenderPass(buffers[frame].getHandle(), &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);
}

void BasicRenderer::updateUniformBuffer(uint32_t currentImage) {
    static auto startTime = std::chrono::high_resolution_clock::now();

    auto currentTime = std::chrono::high_resolution_clock::now();
    float time = std::chrono::duration<float, std::chrono::seconds::period>(currentTime - startTime).count();

    UniformBufferObject ubo{};
    ubo.model = glm::rotate(glm::mat4(1.0f), time * glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f));

    ubo.view = glm::lookAt(glm::vec3(2.0f, 2.0f, 2.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f));

    ubo.proj = glm::perspective(glm::radians(45.0f), swapchain->getSwapChainExtent().width / (float) swapchain->getSwapChainExtent().height, 0.1f, 10.0f);

    ubo.proj[1][1] *= -1;

    memcpy(uniformBuffersMapped[currentImage], &ubo, sizeof(ubo));
}

void BasicRenderer::render(size_t frame, Fence* fence, std::vector<Semaphore*> signalSemaphores, std::vector<Semaphore*> waitSemaphores, std::vector<VkPipelineStageFlags> waitStages) {
    updateUniformBuffer(frame);
    buffers[frame].reset();
    buffers[frame].startRecording();
    beginRenderPass(frame);
    pipeline.bind(&buffers[frame]);
    vertexBuffer.bindVertex(&buffers[frame]);
    indexBuffer.bindIndex(&buffers[frame]);
    
    VkViewport viewport{};
    viewport.x = 0.0f;
    viewport.y = 0.0f;
    viewport.width = swapchain->getSwapChainExtent().width;
    viewport.height = swapchain->getSwapChainExtent().height;
    viewport.minDepth = 0.0f;
    viewport.maxDepth = 1.0f;
    vkCmdSetViewport(buffers[frame].getHandle(), 0, 1, &viewport);

    VkRect2D scissor{};
    scissor.offset = {0, 0};
    scissor.extent = swapchain->getSwapChainExtent();
    vkCmdSetScissor(buffers[frame].getHandle(), 0, 1, &scissor);

    vkCmdBindDescriptorSets(buffers[frame].getHandle(), VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline.getPipelineLayout(), 0, 1, &descriptorSets[frame], 0, nullptr);
    vkCmdDrawIndexed(buffers[frame].getHandle(), static_cast<uint32_t>(indicies.size()), 1, 0, 0, 0);
    vkCmdEndRenderPass(buffers[frame].getHandle());
    buffers[frame].stopRecording();
    buffers[frame].submit(device->getGraphicsQueue(), fence, signalSemaphores, waitSemaphores, waitStages);
}