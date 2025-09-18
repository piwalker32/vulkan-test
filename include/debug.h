#pragma once

#include <vector>
#include <vulkan/vulkan.h>
#include <string.h>

#define DEBUG_INCLUDE_GENERAL_MESSAGES false
#define DEBUG_INCLUDE_PERFORMANCE_MESSAGES true
#define DEBUG_INCLUDE_VALIDATION_MESSAGES true
#define DEBUG_MIN_SEVERITY VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT

const std::vector<const char*> validationLayers = {
    "VK_LAYER_KHRONOS_validation"
};

#ifdef NDEBUG
    const bool enableValidationLayers = false;
#else
    const bool enableValidationLayers = true;
#endif

const char* getMessageSeverityString(VkDebugUtilsMessageSeverityFlagBitsEXT severity);

const char* getMessageTypeString(VkDebugUtilsMessageTypeFlagsEXT type);

VkResult CreateDebugUtilsMessengerEXT(VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo,
                                         const VkAllocationCallbacks* pAllocator, VkDebugUtilsMessengerEXT* pDebugMessenger);

void DestroyDebugUtilsMessengerEXT(VkInstance instance, VkDebugUtilsMessengerEXT debugMessenger,
                                         const VkAllocationCallbacks* pAllocator);

bool checkValidationLayerSupport();