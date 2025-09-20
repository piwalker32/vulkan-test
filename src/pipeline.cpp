#include "shader.h"
#include "swapchain.h"
#include <iostream>
#include <pipeline.h>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>
#include <vulkan/vulkan_core.h>

VkShaderStageFlagBits getStageFromFilename(const char* fileName) {
    std::string name(fileName);
    std::stringstream stream(name);
    std::vector<std::string> tokens;
    std::string temp;
    while(std::getline(stream, temp, '.')) {
        tokens.push_back(temp);
    }

    std::string shaderTypeString = tokens[tokens.size()-2];
    if (shaderTypeString == std::string("vert")) {
        return VK_SHADER_STAGE_VERTEX_BIT;
    } else if (shaderTypeString == std::string("frag")) {
        return VK_SHADER_STAGE_FRAGMENT_BIT;
    }

    return VK_SHADER_STAGE_ALL;
}

Pipeline::Pipeline(Device* device, const std::vector<const char*> shaderFiles, SwapChain* swapchain) :
    device(device) {
    std::vector<Shader> shaders;
    shaders.reserve(shaderFiles.size());
    std::vector<VkPipelineShaderStageCreateInfo> shaderStageCreateInfos;
    shaderStageCreateInfos.reserve(shaderFiles.size());

    for(auto shaderFile : shaderFiles) {
        shaders.emplace_back(device, shaderFile);
        VkPipelineShaderStageCreateInfo createInfo{};
        createInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
        createInfo.stage = getStageFromFilename(shaderFile);
        createInfo.module = shaders.end()->getShader();
        createInfo.pName = "main";
    }

    //Vertex input, empty for now since verticies are hard coded into the shader
    VkPipelineVertexInputStateCreateInfo vertexInputInfo{};
    vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
    vertexInputInfo.vertexBindingDescriptionCount = 0;
    vertexInputInfo.pVertexAttributeDescriptions = nullptr;
    vertexInputInfo.vertexAttributeDescriptionCount = 0;
    vertexInputInfo.pVertexAttributeDescriptions = nullptr;

    //Input Assembly, what types of primitives do we want to draw
    VkPipelineInputAssemblyStateCreateInfo inputAssembly{};
    inputAssembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
    inputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
    inputAssembly.primitiveRestartEnable = VK_FALSE;

    //Viewport, what part of the framebuffer do we want to render to
    VkViewport viewport{};
    viewport.x = 0.0f;
    viewport.y = 0.0f;
    viewport.width = (float) swapchain->getSwapChainExtent().width;
    viewport.height = (float) swapchain->getSwapChainExtent().height;
    viewport.minDepth = 0.0f;
    viewport.maxDepth = 1.0f;

    //Scissor, crop the rendered image
    VkRect2D scissor{};
    scissor.offset = {0, 0};
    scissor.extent = swapchain->getSwapChainExtent();

    //creating static viewport and scissor for now, may make dynamic in future
    VkPipelineViewportStateCreateInfo viewportState{};
    viewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
    viewportState.viewportCount = 1;
    viewportState.pViewports = &viewport;
    viewportState.scissorCount = 1;
    viewportState.pScissors = &scissor;

    //Rasterizer setup
    VkPipelineRasterizationStateCreateInfo rasterizer{};
    rasterizer.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
    rasterizer.depthClampEnable = VK_FALSE; // clamps depth values to the near and far planes instead of discarding
    rasterizer.rasterizerDiscardEnable = VK_FALSE; // disables passing anything past the rasterizer stage
    rasterizer.polygonMode = VK_POLYGON_MODE_FILL; // how do we want polygons to render, fill, or wireframe?
    rasterizer.lineWidth = 1.0f; // sets thickness of lines, any higher than 1.0 requries gpu feature
    rasterizer.cullMode = VK_CULL_MODE_BACK_BIT; // cull the back face
    rasterizer.frontFace = VK_FRONT_FACE_CLOCKWISE; // front face is when verts are defined clockwise
    rasterizer.depthBiasEnable = VK_FALSE; // can alther depth values in some way
    rasterizer.depthBiasConstantFactor = 0.0f;
    rasterizer.depthBiasClamp = 0.0f;
    rasterizer.depthBiasSlopeFactor = 0.0f;

    //multisampling, helpfull for antialiasing disabled for now will revisit later
    VkPipelineMultisampleStateCreateInfo multisampling{};
    multisampling.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
    multisampling.sampleShadingEnable = VK_FALSE; //disabled for now. enabling requires a gpu feature
    multisampling.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
    multisampling.minSampleShading = 1.0f;
    multisampling.pSampleMask = nullptr;
    multisampling.alphaToCoverageEnable = VK_FALSE;
    multisampling.alphaToOneEnable = VK_FALSE;

    //depth and stencil testing, disabled for now will revisit later

    //color blending, how do we mix the new color with the color already in the frambuffer?
    VkPipelineColorBlendAttachmentState colorBlendAttachment{};
    colorBlendAttachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT |
        VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
    colorBlendAttachment.blendEnable = VK_TRUE;
    colorBlendAttachment.srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA;
    colorBlendAttachment.dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
    colorBlendAttachment.colorBlendOp = VK_BLEND_OP_ADD;
    colorBlendAttachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
    colorBlendAttachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
    colorBlendAttachment.alphaBlendOp = VK_BLEND_OP_ADD;

    VkPipelineColorBlendStateCreateInfo colorBlending{};
    colorBlending.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
    colorBlending.logicOpEnable = VK_FALSE;
    colorBlending.logicOp = VK_LOGIC_OP_COPY;
    colorBlending.attachmentCount = 1;
    colorBlending.pAttachments = &colorBlendAttachment;

    //pipeline layout, this defines all uniforms required by pipeline shaders. disabled for now will come back to this later
    VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
    pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    pipelineLayoutInfo.setLayoutCount = 0;
    pipelineLayoutInfo.pSetLayouts = nullptr;
    pipelineLayoutInfo.pushConstantRangeCount = 0;
    pipelineLayoutInfo.pPushConstantRanges = nullptr;

    if(vkCreatePipelineLayout(device->getDevice(), &pipelineLayoutInfo, nullptr, &layout) != VK_SUCCESS) {
        throw std::runtime_error("FAILED TO CREATE PIPELINE LAYOUT");
    }


}

Pipeline::~Pipeline() {

    vkDestroyPipelineLayout(device->getDevice(), layout, nullptr);

}