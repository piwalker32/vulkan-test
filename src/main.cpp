#include "pipeline.h"
#include "shader.h"
#include "surface.h"
#include "swapchain.h"
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
    Surface surface;
    Device device;
    SwapChain swapchain;
    Pipeline pipeline;

public:
    HelloTriangleApplication() :
        instance("Vulkan Test", VK_MAKE_VERSION(1, 0, 0)),
        window(WIDTH, HEIGHT, "Vulkan Test"),
        device(&instance, &surface),
        surface(&instance, &window),
        swapchain(&device, &window, &surface),
        pipeline(&device, { "basic.frag.spv", "basic.vert.spv" }) {
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
    
    try{
        HelloTriangleApplication app;
        app.run();
    } catch (const std::exception& e) {
        std::cerr << "\033[31m" << e.what() << "\033[0m" << std::endl;
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}