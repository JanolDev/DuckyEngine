#pragma once
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <string>

class Window {
public:
    Window(int width, int height, const std::string& title);
    ~Window();

    bool shouldClose() const { return glfwWindowShouldClose(m_window); }
    void close() { glfwSetWindowShouldClose(m_window, GLFW_TRUE); }
    void pollEvents() { glfwPollEvents(); }
    void swapBuffers() { glfwSwapBuffers(m_window); }

    GLFWwindow* getNativeWindow() { return m_window; }
    int getWidth() const { return m_width; }
    int getHeight() const { return m_height; }

private:
    GLFWwindow* m_window;
    int m_width;
    int m_height;
    std::string m_title;
};