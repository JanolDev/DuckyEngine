#include <glad/glad.h>

#include "Window.hpp"
#include <stdexcept>

Window::Window(int width, int height, const char *title) {
    // Initialize GLFW library
    if (!glfwInit())
        throw std::runtime_error("Failed to initialize GLFW");

    // Set OpenGL version and profile hints
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
    // macOS requires forward-compatible core profile
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    // Create a GLFW window with given width, height, and title
    window = glfwCreateWindow(width, height, title, nullptr, nullptr);
    if (!window)
        throw std::runtime_error("Failed to create window");

    // Set the created window as the current OpenGL context
    glfwMakeContextCurrent(window);

    // Load OpenGL function pointers using GLAD
    if (!gladLoadGLLoader((GLADloadproc) glfwGetProcAddress))
        throw std::runtime_error("Failed to initialize GLAD");

    // Set callback to adjust viewport when window is resized
    glfwSetFramebufferSizeCallback(window, framebufferCallback);
}

Window::~Window() {
    // Destroy the GLFW window and terminate GLFW
    glfwDestroyWindow(window);
    glfwTerminate();
}

void Window::framebufferCallback(GLFWwindow *, int width, int height) {
    // Update OpenGL viewport to match new window size
    glViewport(0, 0, width, height);
}

bool Window::shouldClose() const {
    // Check if the window should close
    return glfwWindowShouldClose(window);
}

void Window::closeWindow() const {
    glfwSetWindowShouldClose(window, true);
}

void Window::pollEvents() const {
    // Poll for input events (keyboard, mouse, etc.)
    glfwPollEvents();
}

void Window::swapBuffers() const {
    // Swap the front and back buffers to display rendered content
    glfwSwapBuffers(window);
}

void Window::setBackgroundColor(float r, float g, float b, float alpha) const {
    glClearColor(r, g, b, alpha);
    glClear(GL_COLOR_BUFFER_BIT);
}

GLFWwindow *Window::getHandle() const {
    // Return the raw GLFWwindow pointer
    return window;
}

void Window::setSize(int width, int height) {
    // Set the window size dynamically
    glfwSetWindowSize(window, width, height);
}

// void Window::setWindowFullScreen(bool enable) const {
//     if (enable) {
//         glfwGetWindowPos(window, &windowedPosX, &windowedPosY);
//         glfwGetWindowSize(window, &windowedWidth, &windowedHeight);
//
//         GLFWwindow* monitor= glfwGetPrimaryMonitor();
//     }
//
// }
