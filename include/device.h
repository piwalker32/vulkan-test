#pragma once

#include "surface.h"
#include <optional>
#include <vulkan/vulkan_core.h>
#include <instance.h>
#include <vector>

struct QueueFamilyIndices {
    std::optional<uint32_t> graphicsFamily;
    std::optional<uint32_t> presentFamily;

    bool isComplete() {
        return graphicsFamily.has_value() && presentFamily.has_value();
    }

    bool noPresent() {
        return graphicsFamily.has_value();
    }
};


const std::vector<const char*> deviceExtensions = {
    VK_KHR_SWAPCHAIN_EXTENSION_NAME
};

struct SwapChainSupportDetails {
    VkSurfaceCapabilitiesKHR capabilities;
    std::vector<VkSurfaceFormatKHR> formats;
    std::vector<VkPresentModeKHR> presentModes;
};

class Device {
private:
    Instance* instance;
    Surface* surface;
    VkPhysicalDevice physicalDevice = VK_NULL_HANDLE;
    VkDevice device;
    VkQueue graphicsQueue;
    VkQueue presentQueue = VK_NULL_HANDLE;
public:
    Device(Instance* instance, Surface* surface = nullptr);
    ~Device();
    void waitIdle();
    VkQueue getGraphicsQueue() { return graphicsQueue; }
    VkQueue getPresentQueue() { return presentQueue; }
    VkDevice getDevice() { return device; }
    VkPhysicalDevice getPhysicalDevices() { return physicalDevice; }
    SwapChainSupportDetails getSwapChainDetails();
    QueueFamilyIndices getQueueFamilies();
};