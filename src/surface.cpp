#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <iostream>
#include <stdexcept>
#include <surface.h>
#include <vulkan/vulkan_core.h>

Surface::Surface(Instance* instance, Window* window) {
    this->instance = instance;
    if(glfwCreateWindowSurface(instance->getInstance(), window->getWindow(), nullptr, &surface) != VK_SUCCESS) {
        throw std::runtime_error("UNABLE TO CREATE WINDOW SURFACE!");
    }

    std::cout << "Window Surface Created" << std::endl;
}

Surface::~Surface() {
    vkDestroySurfaceKHR(instance->getInstance(), surface, nullptr);
}