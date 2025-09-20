#include <fstream>
#include <ios>
#include <iostream>
#include <shader.h>
#include <stdexcept>
#include <vector>
#include <vulkan/vulkan_core.h>

static std::vector<char> readFile(const std::string& filename) {
    std::ifstream file(filename, std::ios::ate | std::ios::binary);

    if(!file.is_open()) {
        throw std::runtime_error("FAILED TO OPEN " + filename);
    }

    size_t fileSize = (size_t) file.tellg();
    std::vector<char> buffer(fileSize);

    file.seekg(0);
    file.read(buffer.data(), fileSize);

    file.close();
    return buffer;
}

Shader::Shader(Device* device, const std::string& filename)
    : device(device) {
    std::vector<char> data = readFile("shaders/" + filename);
    
    VkShaderModuleCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
    createInfo.codeSize = data.size();
    createInfo.pCode = reinterpret_cast<const uint32_t*>(data.data());

    if(vkCreateShaderModule(device->getDevice(), &createInfo, nullptr, &shaderModule) != VK_SUCCESS) {
        throw std::runtime_error("FAILED TO CREATE SHADER MODULE");
    }
}

Shader::~Shader() {
    vkDestroyShaderModule(device->getDevice(), shaderModule, nullptr);
}