#include "basic_renderer.h"
#include "fence.h"
#include "global_config.h"
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
    std::vector<Semaphore> imageAvailableSemaphores;
    std::vector<Semaphore> renderFinishedSemaphores;
    std::vector<Fence> inFlightFences;
    uint32_t currentFrame = 0;

public:
    HelloTriangleApplication() :
        instance("Vulkan Test", VK_MAKE_VERSION(1, 0, 0)),
        window(WIDTH, HEIGHT, "Vulkan Test"),
        device(&instance, &surface),
        surface(&instance, &window),
        swapchain(&device, &window, &surface),
        renderer(&device, &swapchain) {
        renderFinishedSemaphores.reserve(swapchain.getImageCount());
        imageAvailableSemaphores.reserve(MAX_FRAMES_IN_FLIGHT);
        inFlightFences.reserve(MAX_FRAMES_IN_FLIGHT);
        for(size_t i = 0; i < swapchain.getImageCount(); i++) {
            renderFinishedSemaphores.emplace_back(&device);
        }
        for(size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
            imageAvailableSemaphores.emplace_back(&device);
            inFlightFences.emplace_back(&device, true);
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
        inFlightFences[currentFrame].wait();
        if(!swapchain.swap(&imageAvailableSemaphores[currentFrame])){
            resize();
            return;
        }
        inFlightFences[currentFrame].reset();
        uint32_t imageIndex = swapchain.getImageIndex();
        renderer.render(currentFrame, &inFlightFences[currentFrame], std::vector<Semaphore*> {&renderFinishedSemaphores[imageIndex]}, 
            std::vector<Semaphore*> {&imageAvailableSemaphores[currentFrame]}, std::vector<VkPipelineStageFlags> {VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT});
        if(!swapchain.present(std::vector<Semaphore*>{&renderFinishedSemaphores[imageIndex]}) || window.getResizedFlag()) {
            resize();
        }

        currentFrame = (currentFrame + 1) % MAX_FRAMES_IN_FLIGHT;
    }

    void resize() {
        device.waitIdle();
        renderer.destroyFramebuffers();
        swapchain.recreateSwapchain();
        renderer.createFramebuffers();
        window.resetResizedFlag();
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