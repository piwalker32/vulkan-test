#include "device.h"
#include "window.h"
#include <algorithm>
#include <cstdint>
#include <iostream>
#include <limits>
#include <stdexcept>
#include <swapchain.h>
#include <vulkan/vulkan_core.h>
#include <vector>

VkSurfaceFormatKHR choseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats) {
    for (const auto& availableFormat : availableFormats) {
        if(availableFormat.format == VK_FORMAT_B8G8R8A8_SRGB && availableFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR ) {
            return availableFormat;
        }
    }

    return availableFormats[0];
}

VkPresentModeKHR choseSwapPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes) {
    for (const auto& mode : availablePresentModes) {
        if(mode == VK_PRESENT_MODE_MAILBOX_KHR) {
            return mode;
        }
    }

    return VK_PRESENT_MODE_FIFO_KHR;
}

VkExtent2D choseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities, Window* window) {
    if (capabilities.currentExtent.width != std::numeric_limits<uint32_t>::max()) {
        return capabilities.currentExtent;
    } else {
        int width = window->getFramebufferWidth();
        int height = window->getFramebufferHeight();

        VkExtent2D actualExtent = {
            static_cast<uint32_t>(width),
            static_cast<uint32_t>(height)
        };

        actualExtent.width = std::clamp(actualExtent.width, capabilities.minImageExtent.width, capabilities.maxImageExtent.width);
        actualExtent.height = std::clamp(actualExtent.height, capabilities.minImageExtent.height, capabilities.maxImageExtent.height);
        return actualExtent;
    }
}

SwapChain::SwapChain(Device* device, Window* window, Surface* surface) :
    device(device), window(window), surface(surface) {
    SwapChainSupportDetails swapChainSupport = device->getSwapChainDetails();

    VkSurfaceFormatKHR surfaceFormat = choseSwapSurfaceFormat(swapChainSupport.formats);
    VkPresentModeKHR presentMode = choseSwapPresentMode(swapChainSupport.presentModes);
    VkExtent2D extent = choseSwapExtent(swapChainSupport.capabilities, window);
    
    swapchainImageFormat = surfaceFormat.format;
    swapchainImageExtent = extent;

    uint32_t imageCount = swapChainSupport.capabilities.minImageCount + 1;

    if(swapChainSupport.capabilities.maxImageCount > 0 && imageCount > swapChainSupport.capabilities.maxImageCount) {
        imageCount = swapChainSupport.capabilities.maxImageCount;
    }

    VkSwapchainCreateInfoKHR createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
    createInfo.surface = surface->getSurface();
    createInfo.minImageCount = imageCount;
    createInfo.imageFormat = surfaceFormat.format;
    createInfo.imageColorSpace = surfaceFormat.colorSpace;
    createInfo.imageExtent = extent;
    createInfo.imageArrayLayers = 1;
    createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

    QueueFamilyIndices indices = device->getQueueFamilies();
    uint32_t queueFamilyIndices[] = {indices.graphicsFamily.value(), indices.presentFamily.value()};

    if(indices.graphicsFamily != indices.presentFamily) {
        createInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
        createInfo.queueFamilyIndexCount = 2;
        createInfo.pQueueFamilyIndices = queueFamilyIndices;
    } else {
        createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
        createInfo.queueFamilyIndexCount = 0;
        createInfo.pQueueFamilyIndices = nullptr;
    }

    createInfo.preTransform = swapChainSupport.capabilities.currentTransform;
    createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
    createInfo.presentMode = presentMode;
    createInfo.clipped = VK_TRUE;

    createInfo.oldSwapchain = VK_NULL_HANDLE;

    if (vkCreateSwapchainKHR(device->getDevice(), &createInfo, nullptr, &swapchain) != VK_SUCCESS) {
        throw std::runtime_error("FAILED TO CREATE SWAPCHAIN!");
    }

    std::cout << "Swapchain created" << std::endl;

    vkGetSwapchainImagesKHR(device->getDevice(), swapchain, &imageCount, nullptr);
    swapChainImages.resize(imageCount);
    vkGetSwapchainImagesKHR(device->getDevice(), swapchain, &imageCount, swapChainImages.data());

    std::cout << "Retrieved swapchain images" << std::endl;

    imageViews.reserve(swapChainImages.size());
    for (size_t i = 0; i < swapChainImages.size(); i++) {
        imageViews.emplace_back(device, swapChainImages[i], swapchainImageFormat, VK_IMAGE_ASPECT_COLOR_BIT);
    }

    std::cout << "Created swapchain imageViews" << std::endl;
}

SwapChain::~SwapChain() {
    vkDestroySwapchainKHR(device->getDevice(), swapchain, nullptr);
}