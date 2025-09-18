#include "surface.h"
#include <cstdint>
#include <device.h>
#include <optional>
#include <set>
#include <stdexcept>
#include <string>
#include <vector>
#include <vulkan/vulkan.h>

#include <iostream>
#include <vulkan/vulkan_core.h>
#include <debug.h>

SwapChainSupportDetails querySwapChainSupport(VkPhysicalDevice device, Surface* surface) {
    SwapChainSupportDetails details;

    vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device, surface->getSurface(), &details.capabilities);

    uint32_t formatCount = 0;
    vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface->getSurface(), &formatCount, nullptr);

    if(formatCount != 0) {
        details.formats.resize(formatCount);
        vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface->getSurface(), &formatCount, details.formats.data());
    }

    uint32_t presentModeCount = 0;
    vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface->getSurface(), &presentModeCount, nullptr);

    if(presentModeCount != 0) {
        details.presentModes.resize(presentModeCount);
        vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface->getSurface(), &presentModeCount, details.presentModes.data());
    }

    return details;
}

QueueFamilyIndices findQueueFamilies(VkPhysicalDevice device, Surface* surface) {
    QueueFamilyIndices indices;

    uint32_t queueFamilyCount = 0;
    vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, nullptr);

    std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
    vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, queueFamilies.data());

    int i = 0;
    for (const auto& queueFamily : queueFamilies) {
        if(queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT) {
            indices.graphicsFamily = i;
        }
        VkBool32 presentSupport = false;
        if(surface != nullptr)
            vkGetPhysicalDeviceSurfaceSupportKHR(device, i, surface->getSurface(), &presentSupport);
        if (presentSupport)
            indices.presentFamily = i;
        
        if(indices.isComplete()) {
            break;
        }
        i++;
    }

    return indices;
}

bool checkDeviceExtensionSupport(VkPhysicalDevice device) {
    uint32_t extensionCount;
    vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, nullptr);

    std::vector<VkExtensionProperties> availableExtensions(extensionCount);
    vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, availableExtensions.data());

    std::set<std::string> requiredExtensions(deviceExtensions.begin(), deviceExtensions.end());
    for (const auto& extension : availableExtensions) {
        requiredExtensions.erase(extension.extensionName);
    }

    return requiredExtensions.empty();
}

bool isDeviceSuitable(VkPhysicalDevice device, Surface* surface) {
    QueueFamilyIndices indices = findQueueFamilies(device, surface);
    bool swapchainAdequate = false;
    if(checkDeviceExtensionSupport(device)) {
        SwapChainSupportDetails swapChainSupport = querySwapChainSupport(device, surface);
        swapchainAdequate = !swapChainSupport.formats.empty() && !swapChainSupport.presentModes.empty();
    }
    return (indices.isComplete() || (indices.noPresent() && surface == nullptr)) && checkDeviceExtensionSupport(device) && swapchainAdequate;
}

VkPhysicalDevice pickPhysicalDevice(Instance* instance, Surface* surface) {
    uint32_t deviceCount = 0;
    vkEnumeratePhysicalDevices(instance->getInstance(), &deviceCount, nullptr);

    if(deviceCount == 0) {
        throw std::runtime_error("FAILED TO FIND VULKAN SUPPORTED GPUS!");
    }

    std::cout << "Found " << deviceCount << " Supported devices" << std::endl;

    std::vector<VkPhysicalDevice> devices(deviceCount);
    vkEnumeratePhysicalDevices(instance->getInstance(), &deviceCount, devices.data());

    VkPhysicalDevice selectedDevice = VK_NULL_HANDLE;

    for(const auto& device : devices) {
        if(isDeviceSuitable(device, surface)) {
            selectedDevice = device;
            break;
        }
    }
    return selectedDevice;
}

Device::Device(Instance* instance,  Surface* surface) :
        surface(surface), instance(instance) {
    physicalDevice = pickPhysicalDevice(instance, surface);
    if(physicalDevice == VK_NULL_HANDLE) {
        throw std::runtime_error("UNABLE TO FIND SUITABLE GPU!");
    }

    VkPhysicalDeviceProperties deviceProperties;
    vkGetPhysicalDeviceProperties(physicalDevice, &deviceProperties);

    std::cout << "Using Device: " << deviceProperties.deviceName << std::endl;

    QueueFamilyIndices indices = findQueueFamilies(physicalDevice, surface);

    std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
    std::vector<uint32_t> queueFamilies = {indices.graphicsFamily.value()};
    if(surface != nullptr)
        queueFamilies.push_back(indices.presentFamily.value());
    std::set<uint32_t> uniqueQueueFamilies(queueFamilies.begin(), queueFamilies.end());
    float queuePriority = 1.0f;

    for(uint32_t queueFamily : uniqueQueueFamilies) {
        VkDeviceQueueCreateInfo queueCreateInfo{};
        queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
        queueCreateInfo.queueFamilyIndex = queueFamily;
        queueCreateInfo.queueCount = 1;
        queueCreateInfo.pQueuePriorities = &queuePriority;
        queueCreateInfos.push_back(queueCreateInfo);
    }

    VkPhysicalDeviceFeatures deviceFeatures{};

    VkDeviceCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
    createInfo.pQueueCreateInfos = queueCreateInfos.data();
    createInfo.queueCreateInfoCount = static_cast<uint32_t>(queueCreateInfos.size());

    createInfo.pEnabledFeatures = &deviceFeatures;
    
    createInfo.enabledExtensionCount = static_cast<uint32_t>(deviceExtensions.size());
    createInfo.ppEnabledExtensionNames = deviceExtensions.data();

    if (enableValidationLayers) {
        createInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());
        createInfo.ppEnabledLayerNames = validationLayers.data();
    } else {
        createInfo.enabledLayerCount = 0;
    }

    if(vkCreateDevice(physicalDevice, &createInfo, nullptr, &device) != VK_SUCCESS) {
        throw std::runtime_error("UNABLE TO CREATE LOGICAL DEVICE!");
    }

    std::cout << "Logical Device Created" << std::endl;

    vkGetDeviceQueue(device, indices.graphicsFamily.value(), 0, &graphicsQueue);
    if(surface != nullptr)
        vkGetDeviceQueue(device, indices.presentFamily.value(), 0, &presentQueue);
}

Device::~Device(){
    vkDestroyDevice(device, nullptr);
}

SwapChainSupportDetails Device::getSwapChainDetails() {
    return querySwapChainSupport(physicalDevice, surface);
}

QueueFamilyIndices Device::getQueueFamilies() {
    return findQueueFamilies(physicalDevice, surface);
}