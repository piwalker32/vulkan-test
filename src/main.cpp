#include <exception>
#include <vulkan/vulkan_core.h>
#include <instance.h>
#include <window.h>
#include <device.h>

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <iostream>
#include <cstdlib>

const uint32_t WIDTH = 800;
const uint32_t HEIGHT = 600;

class HelloTriangleApplication {
private:
    Window window;
    Instance instance;
    Device device;

public:
    HelloTriangleApplication() :
     instance("Vulkan Test", VK_MAKE_VERSION(1, 0, 0)), window(WIDTH, HEIGHT, "Vulkan Test"), device(&instance) {
    }

    void run() {
        mainLoop();
    }    

private:

    void mainLoop() {
        while(!window.shouldClose()) {
            
            window.pollEvents();
        }
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