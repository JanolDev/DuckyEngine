#pragma once
#include <glad/glad.h>

class Viewport {
public:
    float m_Width=0.0f;
    float m_Height=0.0f;
    Viewport(int width, int height);
    ~Viewport();

    // Przygotowanie do rysowania sceny 3D
    void bind();
    void unbind();

    // NOWOŚĆ: Nakładanie efektów
    void drawPostProcess(int effectMode);

    // Pobranie tekstury (teraz zwracamy tę przetworzoną!)
    unsigned int getFinalTexture() const { return postProcessTexture; }

    int getWidth() const { return m_Width; }
    int getHeight() const { return m_Height; }

private:

    // 1. FBO Sceny (Tu rysujemy kaczki i słońce)
    unsigned int sceneFBO;
    unsigned int sceneTexture;
    unsigned int rbo;

    // 2. FBO Post-Processingu (Tu rysujemy wynik z efektem)
    unsigned int postProcessFBO;
    unsigned int postProcessTexture;

    // 3. Screen Quad (Ekran, na którym wyświetlamy shader)
    unsigned int quadVAO, quadVBO;
    unsigned int screenShader;

    void initRenderBuffers();
    void initPostProcessBuffers();
    void initScreenQuad();
    void initScreenShader();
};