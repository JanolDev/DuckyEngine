#pragma once
#include <glad/glad.h>

class TriangleRenderer {
public:
    TriangleRenderer();
    ~TriangleRenderer();

    void draw();

private:
    unsigned int VAO, VBO;
    unsigned int shaderProgram;

    unsigned int compileShader(unsigned int type, const char* source);
    void setupTriangle();
    void setupShader();
};