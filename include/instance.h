#pragma once
#include <vulkan/vulkan.h>

#include <GLFW/glfw3.h>

class Instance {
private:
    VkInstance instance;
    VkDebugUtilsMessengerEXT debugMessenger;
public:
    Instance();
    ~Instance();
private:
    void setupDebugMessenger();
};