#pragma once
#include <GLFW/glfw3.h>

class Window {
private:
    GLFWwindow* window;
    bool resized = false;
public:
    Window(uint32_t width, uint32_t height, const char* title, bool resizable = true);
    ~Window();
    bool shouldClose();
    void pollEvents();
    GLFWwindow* getWindow() { return window; }
    int getFramebufferWidth();
    int getFramebufferHeight();
    bool getResizedFlag() { return resized; }
    void resetResizedFlag() { resized = false; }
    void setResizedFlag() { resized = true; }
};