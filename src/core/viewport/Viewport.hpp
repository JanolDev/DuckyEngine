#pragma once
#include <glad/glad.h>

class Viewport {
public:
    Viewport(int width, int height);
    ~Viewport();

    // Przygotowanie do rysowania sceny 3D
    void bind();
    void unbind();

    // NOWOŚĆ: Nakładanie efektów
    void drawPostProcess(int effectMode);

    // Pobranie tekstury (teraz zwracamy tę przetworzoną!)
    unsigned int getFinalTexture() const { return postProcessTexture; }

private:
    int width, height;

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