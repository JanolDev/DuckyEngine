#pragma once
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <string>

class Window {
public:
    Window(int width, int height, const std::string& title);
    ~Window();

    bool shouldClose();
    void swapBuffers();
    void pollEvents();
    void close(); // Funkcja do zamykania z menu

    // --- NOWOŚĆ: Fullscreen ---
    void toggleFullscreen();

    GLFWwindow* getNativeWindow() const { return window; }
    int getWidth() const { return width; }
    int getHeight() const { return height; }

    void setTitle(const std::string& title);

private:
    GLFWwindow* window;
    int width, height;

    // Zmienne do przywracania okna po wyjściu z fullscreena
    bool isFullscreen = false;
    int windowedPosX, windowedPosY;
    int windowedWidth, windowedHeight;
};