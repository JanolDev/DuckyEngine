#pragma once
#include <GLFW/glfw3.h>

class Window {
public:
    Window(int width, int height, const char* title);
    ~Window();

    bool shouldClose() const;
    void pollEvents() const;
    void swapBuffers() const;

    GLFWwindow* getHandle() const;

    void setSize(int width, int height);


private:
    GLFWwindow* window = nullptr;

    static void framebufferCallback(GLFWwindow* window, int width, int height);
};
