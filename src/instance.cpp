#include <cstring>
#include <instance.h>

#include <vector>
#include <iostream>

#include <debug.h>

std::vector<const char*> getRequiredExtensions() {
    uint32_t glfwExtensionCount = 0;
    const char** glfwExtensions;
    glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

    std::vector<const char*> extensions(glfwExtensions, glfwExtensions+glfwExtensionCount);

    if(enableValidationLayers) {
        extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
    }

    return extensions;
}

VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(
        VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
        VkDebugUtilsMessageTypeFlagsEXT messageType,
        const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
        void* pUserData) {
    
    if(((messageType == VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT && DEBUG_INCLUDE_GENERAL_MESSAGES) || 
        (messageType == VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT && DEBUG_INCLUDE_PERFORMANCE_MESSAGES) ||
        (messageType == VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT && DEBUG_INCLUDE_VALIDATION_MESSAGES)) &&
        messageSeverity >= DEBUG_MIN_SEVERITY) {
        
        std::cerr << "DEBUG MESSAGE [" << getMessageSeverityString(messageSeverity)
                    << "] <" << getMessageTypeString(messageType) << ">: " << pCallbackData->pMessage << std::endl;
    }

    return VK_FALSE;
}

void populateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& createInfo) {
    createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
    createInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT | 
                                    VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
    createInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT;
    createInfo.pfnUserCallback = debugCallback;
    createInfo.pUserData = nullptr;
}

Instance::Instance(const char* appName, uint32_t appVersion, uint32_t apiVersion) {
    uint32_t glfwExtensionCount = 0;
    auto requiredExtensions = getRequiredExtensions();

    uint32_t extensionCount = 0;
    vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, nullptr);
    std::vector<VkExtensionProperties> extensions(extensionCount);
    vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, extensions.data());
    std::cout << "Checking availability of required extensions...";
    for (auto requiredExtension : requiredExtensions) {
        bool found = false;
        for(const auto& extension : extensions) {
            if (strcmp(extension.extensionName, requiredExtension)) {
                found = true;
                break;
            }
        }
        if(!found) {
            std::cout << "NO" << std::endl;
            throw std::runtime_error("REQUIRED VULKAN EXTENSIONS NOT SUPPORTED!");
        }
    }

    std::cout << "YES" << std::endl;

    std::cout << "Validation Layers Requested?..." << (enableValidationLayers ? "YES" : "NO") << std::endl;

    if(enableValidationLayers) {
        std::cout << "Checking Validation Layer Availability...";
        bool available = checkValidationLayerSupport();
        if(!available) {
            std::cout << "NO" << std::endl;
            throw std::runtime_error("Validation layers requested, but not supported");
        }
        std::cout << "YES" << std::endl;
    }


    VkApplicationInfo appInfo{};
    appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    appInfo.pApplicationName = appName;
    appInfo.applicationVersion = appVersion;
    appInfo.pEngineName = "No Engine";
    appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
    appInfo.apiVersion = apiVersion;

    VkInstanceCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    createInfo.pApplicationInfo = &appInfo;

    createInfo.enabledExtensionCount = static_cast<uint32_t>(requiredExtensions.size());
    createInfo.ppEnabledExtensionNames = requiredExtensions.data();
    VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo{};
    if(enableValidationLayers) {
        createInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());
        createInfo.ppEnabledLayerNames = validationLayers.data();

        populateDebugMessengerCreateInfo(debugCreateInfo);
        createInfo.pNext = (VkDebugUtilsMessengerCreateInfoEXT*) &debugCreateInfo;
    } else {
        createInfo.enabledLayerCount = 0;
    }

    if(vkCreateInstance(&createInfo, nullptr, &instance) != VK_SUCCESS) {
        throw std::runtime_error("FAILED TO CREATE VULKAN INSTANCE!");
    }
    std::cout << "Vulkan instance created" << std::endl;

    setupDebugMessenger();
    
}

void Instance::setupDebugMessenger() {
    if(!enableValidationLayers) return;

    VkDebugUtilsMessengerCreateInfoEXT createInfo{};
    populateDebugMessengerCreateInfo(createInfo);


    if(CreateDebugUtilsMessengerEXT(instance, &createInfo, nullptr, &debugMessenger) != VK_SUCCESS) {
        throw std::runtime_error("FAILED TO SETUP DEBUG MESSENGER!");
    }
}

Instance::~Instance() {
    if (enableValidationLayers) {
        DestroyDebugUtilsMessengerEXT(instance, debugMessenger, nullptr);
    }

    vkDestroyInstance(instance, nullptr);
}