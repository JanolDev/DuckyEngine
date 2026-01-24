#pragma once

#include "../window/Window.hpp"

class GuiLayer {
public:
    explicit GuiLayer(Window& window);
    ~GuiLayer();

    void begin();
    void end();
    void setupTheme();
    void drawSplashScreen(unsigned int textureID, float durationSeconds);
    void drawBackground(unsigned int textureID, float alpha = 0.25f);
private:
    Window* m_window;
};
