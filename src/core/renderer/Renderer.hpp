#pragma once
#include <glad/glad.h>
#include <vector>
#include <string>
#include "../sceneobject/SceneObject.hpp"
#include "../math/Mat4.hpp"
#include "../math/Vec3.hpp"

class PrimitiveRenderer {
public:
    PrimitiveRenderer();
    ~PrimitiveRenderer();

    // Rysowanie cieni (Pass 1)
    void drawShadows(const std::vector<SceneObject>& objects, const Vec3& lightPos);

    // Główne rysowanie (Pass 2)
    void draw(const std::vector<SceneObject>& objects, const Mat4& view, const Mat4& proj, const Vec3& cameraPos, const Vec3& lightPos, int selectedId = -1);

    void drawGrid(const Mat4& view, const Mat4& proj);
    void drawSkybox(const Mat4& view, const Mat4& proj);

    unsigned int loadTexture(const std::string& path);
    unsigned int loadCubemap(std::vector<std::string> faces);
    SceneObject loadModel(const std::string& path);

    // Getter do FBO cieni (potrzebne w main.cpp)
    unsigned int getShadowMapFBO() const { return shadowMapFBO; }
    unsigned int getShadowWidth() const { return SHADOW_WIDTH; }
    unsigned int getShadowHeight() const { return SHADOW_HEIGHT; }

private:
    unsigned int vao[4], vbo[4];
    unsigned int shaderProgram; // Główny shader (Phong + Shadows)

    // --- SHADOW MAPPING ---
    unsigned int shadowMapFBO;
    unsigned int shadowMap;     // Tekstura głębokości
    unsigned int depthShader;   // Prosty shader do renderowania z pktu widzenia słońca
    const unsigned int SHADOW_WIDTH = 2048, SHADOW_HEIGHT = 2048; // Rozdzielczość cienia
    Mat4 lightSpaceMatrix;      // Macierz widoku słońca

    // Grid
    unsigned int gridVao, gridVbo, gridShader;
    std::vector<float> gridVertices;

    // Skybox
    unsigned int skyboxVAO, skyboxVBO, skyboxShader;
    unsigned int skyboxTextureID;

    void initGrid();
    void initSkybox();
    void initShadowMap(); // Inicjalizacja buforów cieni

    // Pomocnicza funkcja do rysowania geometrii (żeby nie dublować pętli for)
    void renderSceneGeometry(const std::vector<SceneObject>& objects, unsigned int shader);
};