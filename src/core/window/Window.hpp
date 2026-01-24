#pragma once
#include <GLFW/glfw3.h>

class Window {
public:
    Window(int width, int height, const char *title);

    ~Window();

    bool shouldClose() const;

    void pollEvents() const;

    void swapBuffers() const;

    GLFWwindow *getHandle() const;

private:
    GLFWwindow *window;

    static void frameBufferCallback(GLFWwindow *win, int w, int h);
};
