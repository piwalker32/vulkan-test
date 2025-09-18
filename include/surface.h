#pragma once

#include "instance.h"
#include "window.h"
#include <vulkan/vulkan_core.h>
class Surface {
private:
    VkSurfaceKHR surface;
    Instance* instance;
public:
    Surface(Instance* instance, Window* window);
    ~Surface();
    VkSurfaceKHR getSurface() { return surface; }
};