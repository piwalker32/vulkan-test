#include <GLFW/glfw3.h>
#include <window.h>
#include <iostream>

static void framebufferResizedCallback(GLFWwindow* window, int width, int height) {
    ((Window*)glfwGetWindowUserPointer(window))->setResizedFlag();
}

Window::Window(uint32_t width, uint32_t height, const char* title, bool resizable) {
    if(glfwInit() != GLFW_TRUE) {
        throw std::runtime_error("FAILED TO INIT GLFW!");
    }
    
    std::cout << "GLFW initialized" << std::endl;

    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    glfwWindowHint(GLFW_RESIZABLE, resizable ? GLFW_TRUE : GLFW_FALSE);

    window = glfwCreateWindow(width, height, title, nullptr, nullptr);
    glfwSetWindowUserPointer(window, this);
    glfwSetFramebufferSizeCallback(window, framebufferResizedCallback);

    if(window == NULL) {
        throw std::runtime_error("FAILED TO CREATE GLFW WINDOW!");
    }
    std::cout << "GLFW window created" << std::endl;
}

Window::~Window(){
    glfwDestroyWindow(window);
    glfwTerminate();
}

bool Window::shouldClose() {
    return glfwWindowShouldClose(window);
}

void Window::pollEvents() {
    glfwPollEvents();
}

int Window::getFramebufferWidth() {
    int width;
    glfwGetFramebufferSize(window, &width, nullptr);
    return width;
}

int Window::getFramebufferHeight() {
    int height;
    glfwGetFramebufferSize(window, nullptr, &height);
    return height;
}