#include <glad/glad.h>
#include "Window.hpp"

#include <stdexcept>

Window::Window(int width, int height, const char *title) {
    if (!glfwInit()) {
        throw std::runtime_error("Failed to initialize GLFW");
    }
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    window = glfwCreateWindow(width, height, title, nullptr, nullptr);
    if (!window) {
        throw std::runtime_error("Window creation failed");
    }
    glfwMakeContextCurrent(window);

    if (!gladLoadGLLoader((GLADloadproc) glfwGetProcAddress)) {
        throw std::runtime_error("GLAD init failed");
    }

    glfwSetFramebufferSizeCallback(window, frameBufferCallback);
};

Window::~Window() {
    glfwDestroyWindow(window);
    glfwTerminate();
};

void Window::frameBufferCallback(GLFWwindow *win, const int w, const int h) {
    glViewport(0, 0, w, h);
}

bool Window::shouldClose() const {
    return glfwWindowShouldClose(window);
}

void Window::pollEvents() const {
    return glfwPollEvents();
}

void Window::swapBuffers() const {
    glfwSwapBuffers(window);
}

GLFWwindow* Window::getHandle() const {
    return window;
}
