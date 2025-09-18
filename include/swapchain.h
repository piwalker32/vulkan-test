#pragma once

#include "device.h"
#include "surface.h"
#include "window.h"
#include <vector>
#include <vulkan/vulkan_core.h>
class SwapChain {
private:
    Device* device;
    Window* window;
    Surface* surface;
    VkSwapchainKHR swapchain;
    std::vector<VkImage> swapChainImages;

    VkFormat swapchainImageFormat;
    VkExtent2D swapchainImageExtent;
public:
    SwapChain(Device* device, Window* window, Surface* surface);
    ~SwapChain();
};