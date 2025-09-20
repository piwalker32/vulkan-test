#include "shader.h"
#include <pipeline.h>
#include <sstream>
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

Pipeline::Pipeline(Device* device, const std::vector<const char*> shaderFiles) {
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
}

Pipeline::~Pipeline() {

}