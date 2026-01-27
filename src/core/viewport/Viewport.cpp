#include "Viewport.hpp"
#include <iostream>
#include <vector>

// --- VERTEX SHADER (Prosty prostokąt na cały ekran) ---
const char* screenVShader = R"(
#version 410 core
layout (location = 0) in vec2 aPos;
layout (location = 1) in vec2 aTexCoords;

out vec2 TexCoords;

void main() {
    TexCoords = aTexCoords;
    gl_Position = vec4(aPos.x, aPos.y, 0.0, 1.0);
})";

// --- FRAGMENT SHADER (Tu dzieją się efekty!) ---
const char* screenFShader = R"(
#version 410 core
out vec4 FragColor;
in vec2 TexCoords;

uniform sampler2D screenTexture;
uniform int effectMode; // 0=Normal, 1=Invert, 2=Grayscale, 3=Cinematic, 4=NightVision

void main() {
    vec3 col = texture(screenTexture, TexCoords).rgb;

    // MODE 0: NORMAL
    if(effectMode == 0) {
        FragColor = vec4(col, 1.0);
    }
    // MODE 1: INVERSE (Negatyw)
    else if(effectMode == 1) {
        FragColor = vec4(1.0 - col, 1.0);
    }
    // MODE 2: GRAYSCALE (Czarno-białe)
    else if(effectMode == 2) {
        float average = 0.2126 * col.r + 0.7152 * col.g + 0.0722 * col.b;
        FragColor = vec4(average, average, average, 1.0);
    }
    // MODE 3: CINEMATIC (Winieta + Gamma Correction)
    else if(effectMode == 3) {
        // Gamma Correction (rozjaśnia cienie)
        col = pow(col, vec3(1.0 / 2.2));

        // Vignette (przyciemnianie rogów)
        vec2 uv = TexCoords;
        uv *=  1.0 - uv.yx;
        float vig = uv.x*uv.y * 15.0;
        vig = pow(vig, 0.2);

        FragColor = vec4(col * vig, 1.0);
    }
    // MODE 4: NIGHT VISION (Zielony + Ziarno)
    else if(effectMode == 4) {
        float gray = dot(col, vec3(0.299, 0.587, 0.114));
        vec3 green = vec3(0.0, gray * 1.5, 0.0);

        // Proste ziarno (szum)
        float noise = (fract(sin(dot(TexCoords, vec2(12.9898,78.233)*2.0)) * 43758.5453));

        // Winieta (lornetka)
        vec2 uv = TexCoords - 0.5;
        float dist = length(uv);
        float circle = smoothstep(0.4, 0.45, dist);

        vec3 finalColor = (green + noise * 0.1) * (1.0 - circle);
        FragColor = vec4(finalColor, 1.0);
    }
})";

Viewport::Viewport(int width, int height) : width(width), height(height) {
    initRenderBuffers();      // FBO Sceny
    initPostProcessBuffers(); // FBO Wynikowe
    initScreenQuad();         // Geometria ekranu
    initScreenShader();       // Shader efektów
}

Viewport::~Viewport() {
    glDeleteFramebuffers(1, &sceneFBO);
    glDeleteTextures(1, &sceneTexture);
    glDeleteFramebuffers(1, &postProcessFBO);
    glDeleteTextures(1, &postProcessTexture);
    glDeleteVertexArrays(1, &quadVAO);
    glDeleteBuffers(1, &quadVBO);
}

void Viewport::bind() {
    glBindFramebuffer(GL_FRAMEBUFFER, sceneFBO);
}

void Viewport::unbind() {
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

// KLUCZOWA FUNKCJA: Rysuje scenę z FBO1 na FBO2 używając shadera
void Viewport::drawPostProcess(int effectMode) {
    // 1. Przełączamy się na FBO wynikowe
    glBindFramebuffer(GL_FRAMEBUFFER, postProcessFBO);
    glViewport(0, 0, width, height);
    glClear(GL_COLOR_BUFFER_BIT); // Czyścimy (nie depth, bo to obraz 2D)

    // 2. Używamy shadera efektów
    glUseProgram(screenShader);
    glUniform1i(glGetUniformLocation(screenShader, "screenTexture"), 0);
    glUniform1i(glGetUniformLocation(screenShader, "effectMode"), effectMode);

    // 3. Podpinamy teksturę ze sceny 3D
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, sceneTexture);

    // 4. Rysujemy prostokąt na cały ekran
    glBindVertexArray(quadVAO);
    glDrawArrays(GL_TRIANGLES, 0, 6);

    // 5. Odpinamy
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Viewport::initRenderBuffers() {
    glGenFramebuffers(1, &sceneFBO);
    glBindFramebuffer(GL_FRAMEBUFFER, sceneFBO);

    glGenTextures(1, &sceneTexture);
    glBindTexture(GL_TEXTURE_2D, sceneTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, sceneTexture, 0);

    glGenRenderbuffers(1, &rbo);
    glBindRenderbuffer(GL_RENDERBUFFER, rbo);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, width, height);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rbo);

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        std::cout << "ERROR::FRAMEBUFFER:: Scene FBO is not complete!" << std::endl;
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Viewport::initPostProcessBuffers() {
    glGenFramebuffers(1, &postProcessFBO);
    glBindFramebuffer(GL_FRAMEBUFFER, postProcessFBO);

    glGenTextures(1, &postProcessTexture);
    glBindTexture(GL_TEXTURE_2D, postProcessTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, postProcessTexture, 0);

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        std::cout << "ERROR::FRAMEBUFFER:: PostProcess FBO is not complete!" << std::endl;
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Viewport::initScreenQuad() {
    // Współrzędne prostokąta (X, Y) i Tekstury (U, V)
    float quadVertices[] = {
        // Pozycje   // TexCoords
        -1.0f,  1.0f,  0.0f, 1.0f,
        -1.0f, -1.0f,  0.0f, 0.0f,
         1.0f, -1.0f,  1.0f, 0.0f,

        -1.0f,  1.0f,  0.0f, 1.0f,
         1.0f, -1.0f,  1.0f, 0.0f,
         1.0f,  1.0f,  1.0f, 1.0f
    };
    glGenVertexArrays(1, &quadVAO);
    glGenBuffers(1, &quadVBO);
    glBindVertexArray(quadVAO);
    glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
}

void Viewport::initScreenShader() {
    unsigned int v = glCreateShader(GL_VERTEX_SHADER); glShaderSource(v, 1, &screenVShader, NULL); glCompileShader(v);
    unsigned int f = glCreateShader(GL_FRAGMENT_SHADER); glShaderSource(f, 1, &screenFShader, NULL); glCompileShader(f);
    screenShader = glCreateProgram(); glAttachShader(screenShader, v); glAttachShader(screenShader, f); glLinkProgram(screenShader);
    glDeleteShader(v); glDeleteShader(f);
}