#include <cstdint>
#include <device.h>
#include <optional>
#include <stdexcept>
#include <vector>
#include <vulkan/vulkan.h>

#include <iostream>
#include <vulkan/vulkan_core.h>

struct QueueFamilyIndices {
    std::optional<uint32_t> graphicsFamily;

    bool isComplete() {
        return graphicsFamily.has_value();
    }
};

QueueFamilyIndices findQueueFamilies(VkPhysicalDevice device) {
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
        if(indices.isComplete()) {
            break;
        }
        i++;
    }

    return indices;
}

bool isDeviceSuitable(VkPhysicalDevice device) {
    QueueFamilyIndices indices = findQueueFamilies(device);
    return indices.isComplete();
}

VkPhysicalDevice pickPhysicalDevice(Instance* instance) {
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
        if(isDeviceSuitable(device)) {
            selectedDevice = device;
            break;
        }
    }
    return selectedDevice;
}

Device::Device(Instance* instance) {
    physicalDevice = pickPhysicalDevice(instance);
    if(physicalDevice == VK_NULL_HANDLE) {
        throw std::runtime_error("UNABLE TO FIND SUITABLE GPU!");
    }

    VkPhysicalDeviceProperties deviceProperties;
    vkGetPhysicalDeviceProperties(physicalDevice, &deviceProperties);

    std::cout << "Using Device: " << deviceProperties.deviceName << std::endl;
}

Device::~Device(){

}