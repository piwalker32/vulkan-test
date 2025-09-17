#pragma once
#include <vulkan/vulkan.h>

#include <GLFW/glfw3.h>
#include <vulkan/vulkan_core.h>

class Instance {
private:
    VkInstance instance;
    VkDebugUtilsMessengerEXT debugMessenger;
public:
    Instance(const char* appName, uint32_t appVersion, uint32_t apiVersion = VK_API_VERSION_1_0);
    ~Instance();
private:
    void setupDebugMessenger();
};