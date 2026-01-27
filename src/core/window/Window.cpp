#include "Window.hpp"
#include <iostream>

void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);
}

Window::Window(int width, int height, const std::string& title)
    : width(width), height(height), windowedWidth(width), windowedHeight(height), windowedPosX(100), windowedPosY(100) {

    if (!glfwInit()) {
        std::cerr << "Failed to initialize GLFW" << std::endl;
        return;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    window = glfwCreateWindow(width, height, title.c_str(), NULL, NULL);
    if (window == NULL) {
        std::cerr << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return;
    }

    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cerr << "Failed to initialize GLAD" << std::endl;
        return;
    }
}

bool Window::shouldClose() { return glfwWindowShouldClose(window); }
void Window::swapBuffers() { glfwSwapBuffers(window); }
void Window::pollEvents() { glfwPollEvents(); }
void Window::close() { glfwSetWindowShouldClose(window, true); }

// --- IMPLEMENTACJA FULLSCREEN ---
void Window::toggleFullscreen() {
    if (!isFullscreen) {
        // Zapisz obecną pozycję i rozmiar, żeby móc wrócić
        glfwGetWindowPos(window, &windowedPosX, &windowedPosY);
        glfwGetWindowSize(window, &windowedWidth, &windowedHeight);

        // Pobierz główny monitor i jego tryb wideo
        GLFWmonitor* monitor = glfwGetPrimaryMonitor();
        const GLFWvidmode* mode = glfwGetVideoMode(monitor);

        // Przełącz na fullscreen (na rozmiar monitora)
        glfwSetWindowMonitor(window, monitor, 0, 0, mode->width, mode->height, mode->refreshRate);
        isFullscreen = true;
    } else {
        // Wróć do okna
        glfwSetWindowMonitor(window, nullptr, windowedPosX, windowedPosY, windowedWidth, windowedHeight, 0);
        isFullscreen = false;
    }
}

Window::~Window() {
    glfwDestroyWindow(window);
    glfwTerminate();
}
