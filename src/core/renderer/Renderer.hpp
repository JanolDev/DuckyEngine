#pragma once
#include <glad/glad.h>
#include <vector>
#include "../sceneobject/SceneObject.hpp"
#include "../math/Mat4.hpp"

class PrimitiveRenderer {
public:
    PrimitiveRenderer();
    ~PrimitiveRenderer();

    void draw(const std::vector<SceneObject>& objects, const Mat4& view, const Mat4& proj, int selectedId = -1);

private:
    // Zwiększamy liczbę buforów do 4 (Triangle, Quad, Pyramid, Cube)
    unsigned int vao[4];
    unsigned int vbo[4];
    unsigned int shaderProgram;

    void setupShape(int index, const std::vector<float>& vertices);
};