#include "basic_renderer.h"
#include "fence.h"
#include "pipeline.h"
#include "surface.h"
#include "swapchain.h"
#include <exception>
#include <semaphore.h>
#include <vector>
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
    BasicRenderer renderer;
    Semaphore imageAvailableSemaphore;
    std::vector<Semaphore> renderFinishedSemaphores;
    Fence inFlightFence;

public:
    HelloTriangleApplication() :
        instance("Vulkan Test", VK_MAKE_VERSION(1, 0, 0)),
        window(WIDTH, HEIGHT, "Vulkan Test"),
        device(&instance, &surface),
        surface(&instance, &window),
        swapchain(&device, &window, &surface),
        renderer(&device, &swapchain),
        imageAvailableSemaphore(&device),
        inFlightFence(&device, true) {
        renderFinishedSemaphores.reserve(swapchain.getImageCount());
        for(int i = 0; i < swapchain.getImageCount(); i++) {
            renderFinishedSemaphores.emplace_back(&device);
        }
    }

    void run() {
        mainLoop();
    }    

private:

    void mainLoop() {
        while(!window.shouldClose()) {
            drawFrame();
            window.pollEvents();
        }
        device.waitIdle();
    }

    void drawFrame() {
        inFlightFence.wait();
        inFlightFence.reset();
        swapchain.swap(&imageAvailableSemaphore);
        uint32_t imageIndex = swapchain.getImageIndex();
        renderer.render(&inFlightFence, std::vector<Semaphore*> {&renderFinishedSemaphores[imageIndex]}, 
            std::vector<Semaphore*> {&imageAvailableSemaphore}, std::vector<VkPipelineStageFlags> {VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT});
        swapchain.present(std::vector<Semaphore*>{&renderFinishedSemaphores[imageIndex]});
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