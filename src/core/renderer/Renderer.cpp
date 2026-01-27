#define TINYOBJLOADER_IMPLEMENTATION
#include "tiny_obj_loader.h"
#include "stb_image.h"
#include "Renderer.hpp"
#include <iostream>
#include <vector>

// ==========================================
// 1. SHADERY (Shadows, Phong, Grid, Skybox)
// ==========================================

// --- Depth Shader (Pass 1 - Cienie) ---
const char* depthVShader = R"(
#version 410 core
layout (location = 0) in vec3 aPos;
uniform mat4 lightSpaceMatrix;
uniform mat4 model;
void main() {
    gl_Position = lightSpaceMatrix * model * vec4(aPos, 1.0);
})";
const char* depthFShader = R"(#version 410 core
void main() { /* Głębokość zapisuje się sama */ })";

// --- Main Shader (Pass 2 - Oświetlenie + Cienie) ---
const char* vShader = R"(
#version 410 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoord;

out vec3 FragPos;
out vec3 Normal;
out vec2 TexCoord;
out vec4 FragPosLightSpace;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform mat4 lightSpaceMatrix;

void main() {
    FragPos = vec3(model * vec4(aPos, 1.0));
    Normal = mat3(transpose(inverse(model))) * aNormal;
    TexCoord = aTexCoord;
    FragPosLightSpace = lightSpaceMatrix * vec4(FragPos, 1.0);
    gl_Position = projection * view * vec4(FragPos, 1.0);
})";

const char* fShader = R"(
#version 410 core
out vec4 FragColor;

in vec3 FragPos;
in vec3 Normal;
in vec2 TexCoord;
in vec4 FragPosLightSpace;

uniform vec3 lightPos;
uniform vec3 viewPos;
uniform vec3 objectColor;

uniform sampler2D texture_diffuse;
uniform sampler2D texture_specular;
uniform sampler2D shadowMap;

uniform bool useTexture;
uniform bool useSpecularMap;
uniform float materialShininess;
uniform float materialSpecularStrength;

float ShadowCalculation(vec4 fragPosLightSpace) {
    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
    projCoords = projCoords * 0.5 + 0.5;
    if(projCoords.z > 1.0) return 0.0;
    float currentDepth = projCoords.z;
    vec3 normal = normalize(Normal);
    vec3 lightDir = normalize(lightPos - FragPos);
    float bias = max(0.005 * (1.0 - dot(normal, lightDir)), 0.0005);

    float shadow = 0.0;
    vec2 texelSize = 1.0 / textureSize(shadowMap, 0);
    for(int x = -1; x <= 1; ++x) {
        for(int y = -1; y <= 1; ++y) {
            float pcfDepth = texture(shadowMap, projCoords.xy + vec2(x, y) * texelSize).r;
            shadow += currentDepth - bias > pcfDepth ? 1.0 : 0.0;
        }
    }
    shadow /= 9.0;
    return shadow;
}

void main() {
    vec3 lightColor = vec3(1.0, 0.98, 0.95);
    float ambientStrength = 0.2;
    vec3 ambient = ambientStrength * lightColor;

    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(lightPos - FragPos);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff * lightColor;

    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 reflectDir = reflect(-lightDir, norm);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), materialShininess);
    vec3 specMapColor = vec3(1.0);
    if(useSpecularMap) specMapColor = texture(texture_specular, TexCoord).rgb;
    vec3 specular = materialSpecularStrength * spec * lightColor * specMapColor;

    float shadow = ShadowCalculation(FragPosLightSpace);
    vec3 lighting = (ambient + (1.0 - shadow) * (diffuse + specular)) * objectColor;

    if(useTexture) FragColor = texture(texture_diffuse, TexCoord) * vec4(lighting, 1.0);
    else FragColor = vec4(lighting, 1.0);
})";

// --- Grid & Skybox Shaders ---
const char* gridVShader = R"(#version 410 core
layout (location = 0) in vec3 aPos; uniform mat4 view; uniform mat4 projection;
void main() { gl_Position = projection * view * vec4(aPos, 1.0); })";
const char* gridFShader = R"(#version 410 core
out vec4 FragColor; void main() { FragColor = vec4(0.4, 0.4, 0.4, 1.0); })";

const char* skyboxVShader = R"(#version 410 core
layout (location = 0) in vec3 aPos; out vec3 TexCoords; uniform mat4 view; uniform mat4 projection;
void main() { TexCoords = aPos; vec4 pos = projection * mat4(mat3(view)) * vec4(aPos, 1.0); gl_Position = pos.xyww; })";
const char* skyboxFShader = R"(#version 410 core
out vec4 FragColor; in vec3 TexCoords; uniform samplerCube skybox;
void main() { FragColor = texture(skybox, TexCoords); })";


// ==========================================
// 2. IMPLEMENTACJA KLASY RENDERER
// ==========================================

PrimitiveRenderer::PrimitiveRenderer() {
    // 1. Shadery Phong
    unsigned int v = glCreateShader(GL_VERTEX_SHADER); glShaderSource(v, 1, &vShader, NULL); glCompileShader(v);
    unsigned int f = glCreateShader(GL_FRAGMENT_SHADER); glShaderSource(f, 1, &fShader, NULL); glCompileShader(f);
    shaderProgram = glCreateProgram(); glAttachShader(shaderProgram, v); glAttachShader(shaderProgram, f); glLinkProgram(shaderProgram);
    glDeleteShader(v); glDeleteShader(f);

    // 2. Shadery Depth
    unsigned int dv = glCreateShader(GL_VERTEX_SHADER); glShaderSource(dv, 1, &depthVShader, NULL); glCompileShader(dv);
    unsigned int df = glCreateShader(GL_FRAGMENT_SHADER); glShaderSource(df, 1, &depthFShader, NULL); glCompileShader(df);
    depthShader = glCreateProgram(); glAttachShader(depthShader, dv); glAttachShader(depthShader, df); glLinkProgram(depthShader);
    glDeleteShader(dv); glDeleteShader(df);

    // 3. Cube Geometry
    std::vector<float> cubeVertices = {
        -0.5f,-0.5f,-0.5f, 0.0f,0.0f,-1.0f, 0.0f,0.0f,  0.5f,0.5f,-0.5f, 0.0f,0.0f,-1.0f, 1.0f,1.0f,  0.5f,-0.5f,-0.5f, 0.0f,0.0f,-1.0f, 1.0f,0.0f,
        0.5f,0.5f,-0.5f, 0.0f,0.0f,-1.0f, 1.0f,1.0f,  -0.5f,-0.5f,-0.5f, 0.0f,0.0f,-1.0f, 0.0f,0.0f,  -0.5f,0.5f,-0.5f, 0.0f,0.0f,-1.0f, 0.0f,1.0f,
        -0.5f,-0.5f,0.5f, 0.0f,0.0f,1.0f, 0.0f,0.0f,  0.5f,-0.5f,0.5f, 0.0f,0.0f,1.0f, 1.0f,0.0f,  0.5f,0.5f,0.5f, 0.0f,0.0f,1.0f, 1.0f,1.0f,
        0.5f,0.5f,0.5f, 0.0f,0.0f,1.0f, 1.0f,1.0f,  -0.5f,0.5f,0.5f, 0.0f,0.0f,1.0f, 0.0f,1.0f,  -0.5f,-0.5f,0.5f, 0.0f,0.0f,1.0f, 0.0f,0.0f,
        -0.5f,0.5f,0.5f, -1.0f,0.0f,0.0f, 1.0f,0.0f,  -0.5f,0.5f,-0.5f, -1.0f,0.0f,0.0f, 1.0f,1.0f,  -0.5f,-0.5f,-0.5f, -1.0f,0.0f,0.0f, 0.0f,1.0f,
        -0.5f,-0.5f,-0.5f, -1.0f,0.0f,0.0f, 0.0f,1.0f,  -0.5f,-0.5f,0.5f, -1.0f,0.0f,0.0f, 0.0f,0.0f,  -0.5f,0.5f,0.5f, -1.0f,0.0f,0.0f, 1.0f,0.0f,
        0.5f,0.5f,0.5f, 1.0f,0.0f,0.0f, 1.0f,0.0f,  0.5f,-0.5f,-0.5f, 1.0f,0.0f,0.0f, 0.0f,1.0f,  0.5f,0.5f,-0.5f, 1.0f,0.0f,0.0f, 1.0f,1.0f,
        0.5f,-0.5f,-0.5f, 1.0f,0.0f,0.0f, 0.0f,1.0f,  0.5f,0.5f,0.5f, 1.0f,0.0f,0.0f, 1.0f,0.0f,  0.5f,-0.5f,0.5f, 1.0f,0.0f,0.0f, 0.0f,0.0f,
        -0.5f,-0.5f,-0.5f, 0.0f,-1.0f,0.0f, 0.0f,1.0f,  0.5f,-0.5f,-0.5f, 0.0f,-1.0f,0.0f, 1.0f,1.0f,  0.5f,-0.5f,0.5f, 0.0f,-1.0f,0.0f, 1.0f,0.0f,
        0.5f,-0.5f,0.5f, 0.0f,-1.0f,0.0f, 1.0f,0.0f,  -0.5f,-0.5f,0.5f, 0.0f,-1.0f,0.0f, 0.0f,0.0f,  -0.5f,-0.5f,-0.5f, 0.0f,-1.0f,0.0f, 0.0f,1.0f,
        -0.5f,0.5f,-0.5f, 0.0f,1.0f,0.0f, 0.0f,1.0f,  0.5f,0.5f,0.5f, 0.0f,1.0f,0.0f, 1.0f,0.0f,  0.5f,0.5f,-0.5f, 0.0f,1.0f,0.0f, 1.0f,1.0f,
        0.5f,0.5f,0.5f, 0.0f,1.0f,0.0f, 1.0f,0.0f,  -0.5f,0.5f,0.5f, 0.0f,1.0f,0.0f, 0.0f,0.0f,  -0.5f,0.5f,-0.5f, 0.0f,1.0f,0.0f, 0.0f,1.0f
    };
    glGenVertexArrays(4, vao); glGenBuffers(4, vbo);
    glBindVertexArray(vao[3]); glBindBuffer(GL_ARRAY_BUFFER, vbo[3]);
    glBufferData(GL_ARRAY_BUFFER, cubeVertices.size() * sizeof(float), cubeVertices.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0); glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float))); glEnableVertexAttribArray(1);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float))); glEnableVertexAttribArray(2);

    // 4. Inicjalizacja komponentów (TU BYŁ BŁĄD - brakowało definicji na dole)
    initGrid();
    initSkybox();
    initShadowMap();
}

void PrimitiveRenderer::initShadowMap() {
    glGenFramebuffers(1, &shadowMapFBO);
    glGenTextures(1, &shadowMap);
    glBindTexture(GL_TEXTURE_2D, shadowMap);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
    float borderColor[] = { 1.0, 1.0, 1.0, 1.0 };
    glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);
    glBindFramebuffer(GL_FRAMEBUFFER, shadowMapFBO);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, shadowMap, 0);
    glDrawBuffer(GL_NONE); glReadBuffer(GL_NONE); glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void PrimitiveRenderer::renderSceneGeometry(const std::vector<SceneObject>& objects, unsigned int shader) {
    for (const auto& obj : objects) {
        if(obj.name == "Sun" && shader == depthShader) continue;

        glUniformMatrix4fv(glGetUniformLocation(shader, "model"), 1, GL_FALSE, obj.transform.getModelMatrix().data());

        if (shader == shaderProgram) {
            glUniform1f(glGetUniformLocation(shader, "materialShininess"), obj.material.shininess);
            glUniform1f(glGetUniformLocation(shader, "materialSpecularStrength"), obj.material.specularStrength);

            glActiveTexture(GL_TEXTURE0); glBindTexture(GL_TEXTURE_2D, obj.textureId > 0 ? obj.textureId : 0);
            glUniform1i(glGetUniformLocation(shader, "texture_diffuse"), 0);
            glUniform1i(glGetUniformLocation(shader, "useTexture"), obj.textureId > 0);

            glActiveTexture(GL_TEXTURE1); glBindTexture(GL_TEXTURE_2D, obj.material.specularMapId > 0 ? obj.material.specularMapId : 0);
            glUniform1i(glGetUniformLocation(shader, "texture_specular"), 1);
            glUniform1i(glGetUniformLocation(shader, "useSpecularMap"), obj.material.specularMapId > 0);

            if(obj.id != -1) // Hack na selectedId wewnątrz pętli pomocniczej (można poprawić)
               glUniform3f(glGetUniformLocation(shader, "objectColor"), 1.0f, 1.0f, 1.0f);
        }

        if(obj.type == MeshType::Cube) { glBindVertexArray(vao[3]); glDrawArrays(GL_TRIANGLES, 0, 36); }
        else if(obj.type == MeshType::Model) { glBindVertexArray(obj.vao); glDrawArrays(GL_TRIANGLES, 0, obj.vertexCount); }
    }
}

void PrimitiveRenderer::drawShadows(const std::vector<SceneObject>& objects, const Vec3& lightPos) {
    glUseProgram(depthShader);
    float near_plane = 1.0f, far_plane = 30.0f;
    Mat4 lightProj = MatrixTransform::perspective(90.0f * 0.01745f, 1.0f, near_plane, far_plane);
    Mat4 lightView = MatrixTransform::lookAt(lightPos, Vec3(0,0,0), Vec3(0,1,0));
    lightSpaceMatrix = lightProj * lightView;
    glUniformMatrix4fv(glGetUniformLocation(depthShader, "lightSpaceMatrix"), 1, GL_FALSE, lightSpaceMatrix.data());

    glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
    glBindFramebuffer(GL_FRAMEBUFFER, shadowMapFBO);
    glClear(GL_DEPTH_BUFFER_BIT);
    glCullFace(GL_FRONT);
    renderSceneGeometry(objects, depthShader);
    glCullFace(GL_BACK);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void PrimitiveRenderer::draw(const std::vector<SceneObject>& objects, const Mat4& view, const Mat4& proj, const Vec3& cameraPos, const Vec3& lightPos, int selectedId) {
    glUseProgram(shaderProgram);
    glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "view"), 1, GL_FALSE, view.data());
    glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "projection"), 1, GL_FALSE, proj.data());
    glUniform3f(glGetUniformLocation(shaderProgram, "lightPos"), lightPos.x, lightPos.y, lightPos.z);
    glUniform3f(glGetUniformLocation(shaderProgram, "viewPos"), cameraPos.x, cameraPos.y, cameraPos.z);
    glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "lightSpaceMatrix"), 1, GL_FALSE, lightSpaceMatrix.data());

    glActiveTexture(GL_TEXTURE2); glBindTexture(GL_TEXTURE_2D, shadowMap);
    glUniform1i(glGetUniformLocation(shaderProgram, "shadowMap"), 2);

    for (const auto& obj : objects) {
        glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "model"), 1, GL_FALSE, obj.transform.getModelMatrix().data());

        glUniform1f(glGetUniformLocation(shaderProgram, "materialShininess"), obj.material.shininess);
        glUniform1f(glGetUniformLocation(shaderProgram, "materialSpecularStrength"), obj.material.specularStrength);

        glActiveTexture(GL_TEXTURE0); glBindTexture(GL_TEXTURE_2D, obj.textureId > 0 ? obj.textureId : 0);
        glUniform1i(glGetUniformLocation(shaderProgram, "texture_diffuse"), 0);
        glUniform1i(glGetUniformLocation(shaderProgram, "useTexture"), obj.textureId > 0);

        glActiveTexture(GL_TEXTURE1); glBindTexture(GL_TEXTURE_2D, obj.material.specularMapId > 0 ? obj.material.specularMapId : 0);
        glUniform1i(glGetUniformLocation(shaderProgram, "texture_specular"), 1);
        glUniform1i(glGetUniformLocation(shaderProgram, "useSpecularMap"), obj.material.specularMapId > 0);

        if(obj.id == selectedId) glUniform3f(glGetUniformLocation(shaderProgram, "objectColor"), 1.0f, 0.8f, 0.2f);
        else glUniform3f(glGetUniformLocation(shaderProgram, "objectColor"), 1.0f, 1.0f, 1.0f);

        if(obj.type == MeshType::Cube) { glBindVertexArray(vao[3]); glDrawArrays(GL_TRIANGLES, 0, 36); }
        else if(obj.type == MeshType::Model) { glBindVertexArray(obj.vao); glDrawArrays(GL_TRIANGLES, 0, obj.vertexCount); }
    }
}

// =========================================================
// BRAKUJĄCE FUNKCJE (PRZYWRÓCONE)
// =========================================================

void PrimitiveRenderer::initGrid() {
    for(int i=-20;i<=20;i++){gridVertices.push_back((float)i);gridVertices.push_back(0.f);gridVertices.push_back(-20.f);gridVertices.push_back((float)i);gridVertices.push_back(0.f);gridVertices.push_back(20.f);gridVertices.push_back(-20.f);gridVertices.push_back(0.f);gridVertices.push_back((float)i);gridVertices.push_back(20.f);gridVertices.push_back(0.f);gridVertices.push_back((float)i);}
    unsigned int v=glCreateShader(GL_VERTEX_SHADER);glShaderSource(v,1,&gridVShader,0);glCompileShader(v);unsigned int f=glCreateShader(GL_FRAGMENT_SHADER);glShaderSource(f,1,&gridFShader,0);glCompileShader(f);
    gridShader=glCreateProgram();glAttachShader(gridShader,v);glAttachShader(gridShader,f);glLinkProgram(gridShader);glDeleteShader(v);glDeleteShader(f);
    glGenVertexArrays(1,&gridVao);glGenBuffers(1,&gridVbo);glBindVertexArray(gridVao);glBindBuffer(GL_ARRAY_BUFFER,gridVbo);glBufferData(GL_ARRAY_BUFFER,gridVertices.size()*sizeof(float),gridVertices.data(),GL_STATIC_DRAW);glVertexAttribPointer(0,3,GL_FLOAT,0,3*sizeof(float),0);glEnableVertexAttribArray(0);
}

void PrimitiveRenderer::initSkybox() {
    float sv[]={-1,1,-1,-1,-1,-1,1,-1,-1,1,-1,-1,1,1,-1,-1,1,-1,-1,-1,1,-1,-1,-1,-1,1,-1,-1,1,-1,-1,1,1,-1,-1,1,1,-1,-1,1,-1,1,1,1,1,1,1,1,1,1,-1,1,-1,-1,-1,-1,1,-1,1,1,1,1,1,1,1,1,1,-1,1,-1,-1,1,-1,1,-1,1,1,-1,1,1,1,1,1,1,-1,1,1,-1,1,-1,-1,-1,-1,-1,-1,1,1,-1,-1,1,-1,-1,-1,-1,1,1,-1,1};
    unsigned int v=glCreateShader(GL_VERTEX_SHADER);glShaderSource(v,1,&skyboxVShader,0);glCompileShader(v);unsigned int f=glCreateShader(GL_FRAGMENT_SHADER);glShaderSource(f,1,&skyboxFShader,0);glCompileShader(f);
    skyboxShader=glCreateProgram();glAttachShader(skyboxShader,v);glAttachShader(skyboxShader,f);glLinkProgram(skyboxShader);glDeleteShader(v);glDeleteShader(f);
    glGenVertexArrays(1,&skyboxVAO);glGenBuffers(1,&skyboxVBO);glBindVertexArray(skyboxVAO);glBindBuffer(GL_ARRAY_BUFFER,skyboxVBO);glBufferData(GL_ARRAY_BUFFER,sizeof(sv),&sv,GL_STATIC_DRAW);glVertexAttribPointer(0,3,GL_FLOAT,0,3*sizeof(float),0);glEnableVertexAttribArray(0);
    std::vector<std::string> faces = { "assets/skybox/right.bmp", "assets/skybox/left.bmp", "assets/skybox/top.bmp", "assets/skybox/bottom.bmp", "assets/skybox/front.bmp", "assets/skybox/back.bmp" };
    skyboxTextureID = loadCubemap(faces);
}

void PrimitiveRenderer::drawGrid(const Mat4& v, const Mat4& p) {
    glUseProgram(gridShader);
    glUniformMatrix4fv(glGetUniformLocation(gridShader,"view"),1,0,v.data());
    glUniformMatrix4fv(glGetUniformLocation(gridShader,"projection"),1,0,p.data());
    glBindVertexArray(gridVao);
    glDrawArrays(GL_LINES,0,gridVertices.size()/3);
}

void PrimitiveRenderer::drawSkybox(const Mat4& v, const Mat4& p) {
    glDepthFunc(GL_LEQUAL);
    glUseProgram(skyboxShader);
    glUniformMatrix4fv(glGetUniformLocation(skyboxShader, "view"),1,0,v.data());
    glUniformMatrix4fv(glGetUniformLocation(skyboxShader, "projection"),1,0,p.data());
    glBindVertexArray(skyboxVAO);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_CUBE_MAP, skyboxTextureID);
    glDrawArrays(GL_TRIANGLES, 0, 36);
    glDepthFunc(GL_LESS);
}

unsigned int PrimitiveRenderer::loadTexture(const std::string& path) {
    unsigned int t; glGenTextures(1,&t); int w,h,nr; unsigned char* d=stbi_load(path.c_str(),&w,&h,&nr,0);
    if(d){GLenum f=(nr==4)?GL_RGBA:GL_RGB;glBindTexture(GL_TEXTURE_2D,t);glTexImage2D(GL_TEXTURE_2D,0,f,w,h,0,f,GL_UNSIGNED_BYTE,d);glGenerateMipmap(GL_TEXTURE_2D);stbi_image_free(d);}
    return t;
}

unsigned int PrimitiveRenderer::loadCubemap(std::vector<std::string> faces) {
    unsigned int t; glGenTextures(1,&t); glBindTexture(GL_TEXTURE_CUBE_MAP,t); int w,h,nr;
    for(unsigned int i=0;i<faces.size();i++){stbi_set_flip_vertically_on_load(false);unsigned char* d=stbi_load(faces[i].c_str(),&w,&h,&nr,0);if(d){glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X+i,0,GL_RGB,w,h,0,GL_RGB,GL_UNSIGNED_BYTE,d);stbi_image_free(d);}}
    stbi_set_flip_vertically_on_load(true);
    glTexParameteri(GL_TEXTURE_CUBE_MAP,GL_TEXTURE_MIN_FILTER,GL_LINEAR);glTexParameteri(GL_TEXTURE_CUBE_MAP,GL_TEXTURE_MAG_FILTER,GL_LINEAR);glTexParameteri(GL_TEXTURE_CUBE_MAP,GL_TEXTURE_WRAP_S,GL_CLAMP_TO_EDGE);glTexParameteri(GL_TEXTURE_CUBE_MAP,GL_TEXTURE_WRAP_T,GL_CLAMP_TO_EDGE);glTexParameteri(GL_TEXTURE_CUBE_MAP,GL_TEXTURE_WRAP_R,GL_CLAMP_TO_EDGE);
    return t;
}

SceneObject PrimitiveRenderer::loadModel(const std::string& path) {
    SceneObject newObj; newObj.name = "Model"; newObj.type = MeshType::Model; newObj.transform.scale = Vec3(1,1,1); newObj.modelPath = path;
    tinyobj::attrib_t attrib; std::vector<tinyobj::shape_t> shapes; std::vector<tinyobj::material_t> materials; std::string warn, err;
    if (!tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &err, path.c_str())) { std::cout << "Model Err: " << warn << err << std::endl; return newObj; }
    std::vector<float> data;
    for (const auto& shape : shapes) {
        for (const auto& index : shape.mesh.indices) {
            data.push_back(attrib.vertices[3 * index.vertex_index + 0]); data.push_back(attrib.vertices[3 * index.vertex_index + 1]); data.push_back(attrib.vertices[3 * index.vertex_index + 2]);
            if (index.normal_index >= 0) { data.push_back(attrib.normals[3 * index.normal_index + 0]); data.push_back(attrib.normals[3 * index.normal_index + 1]); data.push_back(attrib.normals[3 * index.normal_index + 2]); } else { data.push_back(0); data.push_back(1); data.push_back(0); }
            if (index.texcoord_index >= 0) { data.push_back(attrib.texcoords[2 * index.texcoord_index + 0]); data.push_back(attrib.texcoords[2 * index.texcoord_index + 1]); } else { data.push_back(0); data.push_back(0); }
        }
    }
    newObj.vertexCount = data.size() / 8; glGenVertexArrays(1, &newObj.vao); unsigned int vbo; glGenBuffers(1, &vbo);
    glBindVertexArray(newObj.vao); glBindBuffer(GL_ARRAY_BUFFER, vbo); glBufferData(GL_ARRAY_BUFFER, data.size() * sizeof(float), data.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0); glEnableVertexAttribArray(0); glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float))); glEnableVertexAttribArray(1); glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float))); glEnableVertexAttribArray(2);
    return newObj;
}

PrimitiveRenderer::~PrimitiveRenderer() {}