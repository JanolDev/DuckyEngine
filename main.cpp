#include <GLFW/glfw3.h>
#include <iostream>

#include "glad/glad.h"


//“Here is exactly how the GPU shall draw this triangle, with no assumptions, no magic, no MERCY.”

//screen resolution
int WIDTH = 800;
int HEIGHT = 600;

//change resolution of the viewport according to changes in window size
void framebuffer_size_callback(GLFWwindow *window, int width, int height) {
    glViewport(0, 0, width, height);
}

int main() {
    //Initialize Window and by it we load all libraries of glfw
    //We define version of the glfw major and minor
    //we initialize profile with all core libraries
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    //macOS requires forward-compatible core profile for OpenGL
    //This removes deprecated functions satisfy macOS's OpenGL requirements
#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    //Create a windowed mode window with given width, height, and title
    //last two parameters: first: windowed window, not fullscreen; second: no shared context
    GLFWwindow *window = glfwCreateWindow(WIDTH, HEIGHT, "Hello Window", nullptr, nullptr);

    //If by making a window we get some bugs or failures we  terminate the process
    if (!window) {
        std::cout << "Failed to create GLFW window\n";
        glfwTerminate();
        return -1;
    }

    // We make our current/main context
    glfwMakeContextCurrent(window);

    //If glad was not sucessfully initialized we end pro
    if (!gladLoadGLLoader((GLADloadproc) glfwGetProcAddress)) {
        std::cout << "Failed to initialize GLAD\n";
        return -1;
    }

    //Setting the size of the viewport
    glViewport(0, 0, WIDTH, HEIGHT);


    //Executing viewport resizing according to the size of the window
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    //checks if app should be closed or not
    //changing pixel colors according to executed functions
    //listener to keyboard and mouse inputs
    while (!glfwWindowShouldClose(window)) {
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    //terminate all app processes
    glfwTerminate();
    return 0;
}