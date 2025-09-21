#pragma once

#include "device.h"
#include "imageview.h"
#include "surface.h"
#include "window.h"
#include <semaphore.h>
#include <vector>
#include <vulkan/vulkan_core.h>
class SwapChain {
private:
    Device* device;
    Window* window;
    Surface* surface;
    VkSwapchainKHR swapchain;
    std::vector<VkImage> swapChainImages;
    std::vector<ImageView> imageViews;
    uint32_t imageIndex;

    VkFormat swapchainImageFormat;
    VkExtent2D swapchainImageExtent;
public:
    SwapChain(Device* device, Window* window, Surface* surface);
    ~SwapChain();
    void recreateSwapchain();
    bool swap(Semaphore* semaphore = nullptr);
    bool present(std::vector<Semaphore*> waitSemaphores = {});
    uint32_t getImageIndex() { return imageIndex; }
    VkExtent2D getSwapChainExtent() { return swapchainImageExtent; }
    VkFormat getSwapChainFormat() { return swapchainImageFormat; }
    size_t getImageCount() { return imageViews.size(); }
    ImageView* getImageView(size_t index) { return &imageViews[index]; }
private:
    void deleteSwapchain();
    void initSwapchain();
};