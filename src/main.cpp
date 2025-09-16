#include <algorithm>
#include <cstring>
#include <exception>
#include <vector>
#include <vulkan/vulkan_core.h>
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <iostream>
#include <stdexcept>
#include <cstdlib>

const uint32_t WIDTH = 800;
const uint32_t HEIGHT = 600;

const std::vector<const char*> validationLayers = {
    "VK_LAYER_KHRONOS_validation"
};

#ifdef NDEBUG
    const bool enableValidationLayers = false;
#else
    const bool enableValidationLayers = true;
#endif

class HelloTriangleApplication {
private:
    GLFWwindow* window;
    VkInstance instance;

public:
    void run() {
        initWindow();
        initVulkan();
        mainLoop();
        cleanup();
    }    

private:
    void initWindow() {
        if(glfwInit() != GLFW_TRUE) {
            throw std::runtime_error("FAILED TO INIT GLFW!");
        }
        
        std::cout << "GLFW initialized" << std::endl;

        glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
        glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

        window = glfwCreateWindow(WIDTH, HEIGHT, "Vulkan Test", nullptr, nullptr);

        if(window == NULL) {
            throw std::runtime_error("FAILED TO CREATE GLFW WINDOW!");
        }
        std::cout << "GLFW window created" << std::endl;
    }

    void initVulkan() {
        createInstance();
    }

    bool checkValidationLayerSupport() {
        uint32_t layerCount;
        vkEnumerateInstanceLayerProperties(&layerCount, nullptr);

        std::vector<VkLayerProperties> availableLayers(layerCount);
        vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data());

        for(const char* layerName : validationLayers) {
            bool found = false;
            for(const auto& layerProperties : availableLayers) {
                if(strcmp(layerName, layerProperties.layerName)) {
                    found = true;
                    break;
                }
            }
            if(!found) {
                return false;
            }
        }

        return true;
    }

    void createInstance() {
        uint32_t glfwExtensionCount = 0;
        const char** glfwExtensions;
        glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

        uint32_t extensionCount = 0;
        vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, nullptr);
        std::vector<VkExtensionProperties> extensions(extensionCount);
        vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, extensions.data());
        std::cout << "Checking availability of required extensions...";
        for (int i = 0; i < glfwExtensionCount; i++) {
            const char* glfwExtension = glfwExtensions[i];
            bool found = false;
            for(const auto& extension : extensions) {
                if (strcmp(extension.extensionName, glfwExtension)) {
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
        appInfo.pApplicationName = "Hello Triangle";
        appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
        appInfo.pEngineName = "No Engine";
        appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
        appInfo.apiVersion = VK_API_VERSION_1_0;

        VkInstanceCreateInfo createInfo{};
        createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
        createInfo.pApplicationInfo = &appInfo;

        createInfo.enabledExtensionCount = glfwExtensionCount;
        createInfo.ppEnabledExtensionNames = glfwExtensions;

        if(enableValidationLayers) {
            createInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());
            createInfo.ppEnabledLayerNames = validationLayers.data();
        } else {
            createInfo.enabledLayerCount = 0;
        }

        if(vkCreateInstance(&createInfo, nullptr, &instance) != VK_SUCCESS) {
            throw std::runtime_error("FAILED TO CREATE VULKAN INSTANCE!");
        }
        std::cout << "Vulkan instance created" << std::endl;
        
    }

    void mainLoop() {
        while(!glfwWindowShouldClose(window)) {
            glfwPollEvents();
        }
    }

    void cleanup() {
        vkDestroyInstance(instance, nullptr);

        glfwDestroyWindow(window);

        glfwTerminate();
    }
};

int main() {
    HelloTriangleApplication app;

    try{
        app.run();
    } catch (const std::exception& e) {
        std::cerr << e.what() << std::endl;
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}