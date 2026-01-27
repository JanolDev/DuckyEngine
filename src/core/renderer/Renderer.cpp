#include "Renderer.hpp"
#include <iostream>

const char* vShader = R"(
#version 410 core
layout (location = 0) in vec3 aPos;
uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
void main() {
    gl_Position = projection * view * model * vec4(aPos, 1.0);
})";

const char* fShader = R"(
#version 410 core
out vec4 FragColor;
uniform vec3 objectColor;
void main() {
    FragColor = vec4(objectColor, 1.0);
})";

PrimitiveRenderer::PrimitiveRenderer() {
    // Kompilacja Shaderów
    unsigned int vertex = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertex, 1, &vShader, nullptr);
    glCompileShader(vertex);

    unsigned int fragment = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragment, 1, &fShader, nullptr);
    glCompileShader(fragment);

    shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertex);
    glAttachShader(shaderProgram, fragment);
    glLinkProgram(shaderProgram);

    glDeleteShader(vertex);
    glDeleteShader(fragment);

    // --- DEFINICJE FIGUR ---

    // 0. Trójkąt
    std::vector<float> tri = { -0.5f, -0.5f, 0.0f, 0.5f, -0.5f, 0.0f, 0.0f, 0.5f, 0.0f };

    // 1. Kwadrat (Quad)
    std::vector<float> quad = {
        -0.5f, 0.5f, 0.0f, -0.5f, -0.5f, 0.0f, 0.5f, -0.5f, 0.0f,
        -0.5f, 0.5f, 0.0f, 0.5f, -0.5f, 0.0f, 0.5f, 0.5f, 0.0f
    };

    // 2. Piramida
    std::vector<float> pyr = {
        -0.5f, 0.0f, -0.5f,  0.5f, 0.0f, -0.5f,  0.5f, 0.0f,  0.5f,
        -0.5f, 0.0f, -0.5f,  0.5f, 0.0f,  0.5f, -0.5f, 0.0f,  0.5f,
        -0.5f, 0.0f, -0.5f,  0.5f, 0.0f, -0.5f,  0.0f, 0.8f,  0.0f,
         0.5f, 0.0f, -0.5f,  0.5f, 0.0f,  0.5f,  0.0f, 0.8f,  0.0f,
         0.5f, 0.0f,  0.5f, -0.5f, 0.0f,  0.5f,  0.0f, 0.8f,  0.0f,
        -0.5f, 0.0f,  0.5f, -0.5f, 0.0f, -0.5f,  0.0f, 0.8f,  0.0f
    };

    // 3. Sześcian (CUBE) - 36 wierzchołków
    std::vector<float> cube = {
        -0.5f, -0.5f, -0.5f,  0.5f, -0.5f, -0.5f,  0.5f,  0.5f, -0.5f,
         0.5f,  0.5f, -0.5f, -0.5f,  0.5f, -0.5f, -0.5f, -0.5f, -0.5f,
        -0.5f, -0.5f,  0.5f,  0.5f, -0.5f,  0.5f,  0.5f,  0.5f,  0.5f,
         0.5f,  0.5f,  0.5f, -0.5f,  0.5f,  0.5f, -0.5f, -0.5f,  0.5f,
        -0.5f,  0.5f,  0.5f, -0.5f,  0.5f, -0.5f, -0.5f, -0.5f, -0.5f,
        -0.5f, -0.5f, -0.5f, -0.5f, -0.5f,  0.5f, -0.5f,  0.5f,  0.5f,
         0.5f,  0.5f,  0.5f,  0.5f,  0.5f, -0.5f,  0.5f, -0.5f, -0.5f,
         0.5f, -0.5f, -0.5f,  0.5f, -0.5f,  0.5f,  0.5f,  0.5f,  0.5f,
        -0.5f, -0.5f, -0.5f,  0.5f, -0.5f, -0.5f,  0.5f, -0.5f,  0.5f,
         0.5f, -0.5f,  0.5f, -0.5f, -0.5f,  0.5f, -0.5f, -0.5f, -0.5f,
        -0.5f,  0.5f, -0.5f,  0.5f,  0.5f, -0.5f,  0.5f,  0.5f,  0.5f,
         0.5f,  0.5f,  0.5f, -0.5f,  0.5f,  0.5f, -0.5f,  0.5f, -0.5f
    };

    setupShape(0, tri);
    setupShape(1, quad);
    setupShape(2, pyr);
    setupShape(3, cube); // Rejestracja sześcianu
}

void PrimitiveRenderer::setupShape(int index, const std::vector<float>& vertices) {
    glGenVertexArrays(1, &vao[index]);
    glGenBuffers(1, &vbo[index]);

    glBindVertexArray(vao[index]);
    glBindBuffer(GL_ARRAY_BUFFER, vbo[index]);
    glBufferData(GL_ARRAY_BUFFER, static_cast<GLsizeiptr>(vertices.size() * sizeof(float)), vertices.data(), GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), nullptr);
    glEnableVertexAttribArray(0);
    glBindVertexArray(0);
}

void PrimitiveRenderer::draw(const std::vector<SceneObject>& objects, const Mat4& view, const Mat4& proj, int selectedId) {
    glUseProgram(shaderProgram);
    glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "view"), 1, GL_FALSE, view.data());
    glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "projection"), 1, GL_FALSE, proj.data());

    int colorLoc = glGetUniformLocation(shaderProgram, "objectColor");

    for (const auto& obj : objects) {
        glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "model"), 1, GL_FALSE, obj.transform.getModelMatrix().data());

        if (obj.id == selectedId) {
            glUniform3f(colorLoc, 1.0f, 0.9f, 0.0f); // Podświetlenie
        } else {
            glUniform3f(colorLoc, 0.0f, 0.6f, 0.9f); // Domyślny kolor (niebieskawy)
        }

        int idx = static_cast<int>(obj.type);
        glBindVertexArray(vao[idx]);

        // Liczba wierzchołków dla każdej figury
        int counts[] = {3, 6, 18, 36};
        glDrawArrays(GL_TRIANGLES, 0, counts[idx]);
    }
    glBindVertexArray(0);
}

PrimitiveRenderer::~PrimitiveRenderer() {
    glDeleteVertexArrays(4, vao);
    glDeleteBuffers(4, vbo);
    glDeleteProgram(shaderProgram);
}