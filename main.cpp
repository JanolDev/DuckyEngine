#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <imgui.h>
#include <vector>
#include <string>
#include <cstring>
#include <cmath>
#include <algorithm>
#include <fstream>
#include <iostream>

#include "ImGuizmo.h"
#include "src/core/json.hpp"
#include "src/core/window/Window.hpp"
#include "src/core/gui/GuiLayer.hpp"
#include "src/core/gui/Console.hpp"
#include "src/core/menubar/MenuBar.hpp"
#include "src/core/camera/Camera.hpp"
#include "src/core/renderer/Renderer.hpp"
#include "src/core/viewport/Viewport.hpp"
#include "src/core/sceneobject/SceneObject.hpp"
#include "src/core/math/Vec4.hpp"

using json = nlohmann::json;

extern "C" char const * tinyfd_openFileDialog(char const * aTitle, char const * aDefaultPathAndFile, int aNumOfFilterPatterns, char const * const * aFilterPatterns, char const * aSingleFilterDescription, int aAllowMultipleSelects);



// --- DODANE STAŁE POMOCNICZE (Naprawa rotacji i narrowing error) ---
const float PI_F = 3.1415926535f;
const float DEG2RAD = PI_F / 180.0f;
const float RAD2DEG = 180.0f / PI_F;

Vec4 multiply(const Mat4& m, const Vec4& v) {
    const float* p = m.data();
    return Vec4(p[0]*v.x+p[4]*v.y+p[8]*v.z+p[12]*v.w, p[1]*v.x+p[5]*v.y+p[9]*v.z+p[13]*v.w, p[2]*v.x+p[6]*v.y+p[10]*v.z+p[14]*v.w, p[3]*v.x+p[7]*v.y+p[11]*v.z+p[15]*v.w);
}

struct GeneratedMesh { unsigned int vao; int vertexCount; };


void addVert(std::vector<float>& v, float x, float y, float z, float nx, float ny, float nz, float u, float tex_v) {
    v.push_back(x); v.push_back(y); v.push_back(z);    // Pos
    v.push_back(nx); v.push_back(ny); v.push_back(nz); // Norm
    v.push_back(u); v.push_back(tex_v);                // UV
}

GeneratedMesh generateSphereMesh(int sectors, int stacks) {
    std::vector<float> data;
    float radius = 0.5f;

    auto getPoint = [&](int i, int j) -> std::vector<float> {
        float stackAngle = PI_F / 2 - (float)i / stacks * PI_F;
        float sectorAngle = (float)j / sectors * 2 * PI_F;

        float xy = radius * cosf(stackAngle);
        float z = xy * cosf(sectorAngle);
        float x = xy * sinf(sectorAngle);
        float y = radius * sinf(stackAngle);

        float u = (float)j / sectors;
        float v = (float)i / stacks;

        return {x, y, z, x/radius, y/radius, z/radius, u, v};
    };

    for(int i = 0; i < stacks; ++i) {
        for(int j = 0; j < sectors; ++j) {
            auto p1 = getPoint(i, j);         // Top Left
            auto p2 = getPoint(i + 1, j);     // Bottom Left
            auto p3 = getPoint(i, j + 1);     // Top Right
            auto p4 = getPoint(i + 1, j + 1); // Bottom Right

            if (i != 0) {
                addVert(data, p1[0], p1[1], p1[2], p1[3], p1[4], p1[5], p1[6], p1[7]);
                addVert(data, p2[0], p2[1], p2[2], p2[3], p2[4], p2[5], p2[6], p2[7]);
                addVert(data, p3[0], p3[1], p3[2], p3[3], p3[4], p3[5], p3[6], p3[7]);
            }

            if (i != (stacks - 1)) {
                addVert(data, p3[0], p3[1], p3[2], p3[3], p3[4], p3[5], p3[6], p3[7]);
                addVert(data, p2[0], p2[1], p2[2], p2[3], p2[4], p2[5], p2[6], p2[7]);
                addVert(data, p4[0], p4[1], p4[2], p4[3], p4[4], p4[5], p4[6], p4[7]);
            }
        }
    }

    unsigned int VAO, VBO;
    glGenVertexArrays(1, &VAO); glGenBuffers(1, &VBO);
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, data.size() * sizeof(float), data.data(), GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0); glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float))); glEnableVertexAttribArray(1);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float))); glEnableVertexAttribArray(2);

    return {VAO, (int)(data.size() / 8)};
}


GeneratedMesh generateCylinderMesh(int sectors) {
    std::vector<float> data;
    float radius = 0.5f;
    float halfH = 0.5f;


    for(int i = 0; i < sectors; ++i) {
        float angle1 = (float)i / sectors * 2.0f * PI_F;
        float angle2 = (float)(i + 1) / sectors * 2.0f * PI_F;

        float x1 = cos(angle1) * radius; float z1 = sin(angle1) * radius;
        float x2 = cos(angle2) * radius; float z2 = sin(angle2) * radius;
        float u1 = (float)i / sectors; float u2 = (float)(i + 1) / sectors;

        addVert(data, x1, halfH, z1, x1/radius, 0, z1/radius, u1, 1.0f);
        addVert(data, x1, -halfH, z1, x1/radius, 0, z1/radius, u1, 0.0f);
        addVert(data, x2, halfH, z2, x2/radius, 0, z2/radius, u2, 1.0f);

        addVert(data, x2, halfH, z2, x2/radius, 0, z2/radius, u2, 1.0f);
        addVert(data, x1, -halfH, z1, x1/radius, 0, z1/radius, u1, 0.0f);
        addVert(data, x2, -halfH, z2, x2/radius, 0, z2/radius, u2, 0.0f);
    }


    for(int i = 0; i < sectors; ++i) {
        float angle1 = (float)i / sectors * 2.0f * PI_F;
        float angle2 = (float)(i + 1) / sectors * 2.0f * PI_F;
        float x1 = cos(angle1) * radius; float z1 = sin(angle1) * radius;
        float x2 = cos(angle2) * radius; float z2 = sin(angle2) * radius;

        addVert(data, 0, halfH, 0, 0, 1, 0, 0.5f, 0.5f);
        addVert(data, x1, halfH, z1, 0, 1, 0, (x1/radius+1)*0.5f, (z1/radius+1)*0.5f);
        addVert(data, x2, halfH, z2, 0, 1, 0, (x2/radius+1)*0.5f, (z2/radius+1)*0.5f);
    }


    for(int i = 0; i < sectors; ++i) {
        float angle1 = (float)i / sectors * 2.0f * PI_F;
        float angle2 = (float)(i + 1) / sectors * 2.0f * PI_F;
        float x1 = cos(angle1) * radius; float z1 = sin(angle1) * radius;
        float x2 = cos(angle2) * radius; float z2 = sin(angle2) * radius;

        addVert(data, 0, -halfH, 0, 0, -1, 0, 0.5f, 0.5f);
        addVert(data, x2, -halfH, z2, 0, -1, 0, (x2/radius+1)*0.5f, (z2/radius+1)*0.5f);
        addVert(data, x1, -halfH, z1, 0, -1, 0, (x1/radius+1)*0.5f, (z1/radius+1)*0.5f);
    }

    unsigned int VAO, VBO;
    glGenVertexArrays(1, &VAO); glGenBuffers(1, &VBO);
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, data.size() * sizeof(float), data.data(), GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0); glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float))); glEnableVertexAttribArray(1);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float))); glEnableVertexAttribArray(2);

    return {VAO, (int)(data.size() / 8)};
}

// --- FIZYKA ---
struct AABB { Vec3 min; Vec3 max; };
AABB getBounds(const SceneObject& o) { float hx=0.5f*o.transform.scale.x, hy=0.5f*o.transform.scale.y, hz=0.5f*o.transform.scale.z; return {Vec3(o.transform.position.x-hx, o.transform.position.y-hy, o.transform.position.z-hz), Vec3(o.transform.position.x+hx, o.transform.position.y+hy, o.transform.position.z+hz)}; }
bool checkCollision(const AABB& a, const AABB& b) { return (a.min.x<=b.max.x && a.max.x>=b.min.x) && (a.min.y<=b.max.y && a.max.y>=b.min.y) && (a.min.z<=b.max.z && a.max.z>=b.min.z); }
bool checkSceneCollision(const SceneObject& a, const std::vector<SceneObject>& objs) { AABB ab=getBounds(a); for(const auto& o:objs) { if(o.id==a.id || !o.hasCollider) continue; if(checkCollision(ab, getBounds(o))) return true; } return false; }
int shootRay(const Vec3& org, const Vec3& dir, const std::vector<SceneObject>& objs) { int hit=-1; float minD=1000.0f; for(const auto& o:objs) { if(!o.hasCollider || o.name=="Player") continue; Vec3 otc=o.transform.position-org; float p=otc.dot(dir); if(p<0) continue; Vec3 pr=org+dir*p; float d=(o.transform.position-pr).length(); if(d < std::max(o.transform.scale.x,o.transform.scale.y)*0.7f) { if(p<minD) { minD=p; hit=o.id; } } } return hit; }

void updatePhysics(std::vector<SceneObject>& objects, float dt) {
    for (auto& obj : objects) {
        if (obj.useGravity && !obj.lockY) obj.velocity.y -= 9.81f * dt;
        if (obj.name != "Player" && std::abs(obj.velocity.x) < 0.001f && std::abs(obj.velocity.y) < 0.001f && std::abs(obj.velocity.z) < 0.001f) continue;
        if (!obj.lockY) { float dY = obj.velocity.y * dt; obj.transform.position.y += dY; if (obj.hasCollider && checkSceneCollision(obj, objects)) { obj.transform.position.y -= dY; obj.velocity.y = 0; } }
        if (!obj.lockX) { float dX = obj.velocity.x * dt; obj.transform.position.x += dX; if (obj.hasCollider && checkSceneCollision(obj, objects)) { obj.transform.position.x -= dX; obj.velocity.x = 0; } }
        if (!obj.lockZ) { float dZ = obj.velocity.z * dt; obj.transform.position.z += dZ; if (obj.hasCollider && checkSceneCollision(obj, objects)) { obj.transform.position.z -= dZ; obj.velocity.z = 0; } }
        if(obj.name != "Player") { obj.velocity.x *= 0.95f; obj.velocity.z *= 0.95f; }
    }
}

// --- SERIALIZATION ---
json serializeObject(const SceneObject& o) {
    json j; j["id"]=o.id; j["name"]=o.name; j["type"]=(int)o.type; j["transform"]["pos"]={o.transform.position.x,o.transform.position.y,o.transform.position.z}; j["transform"]["rot"]={o.transform.rotation.x,o.transform.rotation.y,o.transform.rotation.z}; j["transform"]["scale"]={o.transform.scale.x,o.transform.scale.y,o.transform.scale.z}; j["hasCollider"]=o.hasCollider; j["useGravity"]=o.useGravity; j["canShoot"]=o.canShoot; j["velocity"]={o.velocity.x,o.velocity.y,o.velocity.z}; j["locks"]={o.lockX,o.lockY,o.lockZ}; j["texturePath"]=o.texturePath; j["modelPath"]=o.modelPath; j["material"]["shininess"]=o.material.shininess; j["material"]["specularStrength"]=o.material.specularStrength; j["material"]["specularMapPath"]=o.material.specularMapPath; return j;
}
SceneObject deserializeObject(const json& e, PrimitiveRenderer& r) {
    SceneObject o; o.id=e.value("id",0); o.name=e.value("name","Obj"); o.type=(MeshType)e.value("type",0);
    if(e.contains("transform")){ auto& t=e["transform"]; if(t.contains("pos")) o.transform.position=Vec3(t["pos"][0],t["pos"][1],t["pos"][2]); if(t.contains("rot")) o.transform.rotation=Vec3(t["rot"][0],t["rot"][1],t["rot"][2]); if(t.contains("scale")) o.transform.scale=Vec3(t["scale"][0],t["scale"][1],t["scale"][2]); }
    o.hasCollider=e.value("hasCollider",true); o.useGravity=e.value("useGravity",false); o.canShoot=e.value("canShoot",false);
    if(e.contains("velocity")) o.velocity=Vec3(e["velocity"][0],e["velocity"][1],e["velocity"][2]);
    if(e.contains("locks")) { o.lockX=e["locks"][0]; o.lockY=e["locks"][1]; o.lockZ=e["locks"][2]; }
    o.texturePath=e.value("texturePath",""); o.modelPath=e.value("modelPath","");
    if(e.contains("material")) { o.material.shininess=e["material"].value("shininess",32.0f); o.material.specularStrength=e["material"].value("specularStrength",0.5f); o.material.specularMapPath=e["material"].value("specularMapPath",""); }
    if(o.type==MeshType::Model && !o.modelPath.empty()) { SceneObject m=r.loadModel(o.modelPath); o.vao=m.vao; o.vertexCount=m.vertexCount; }
    if(!o.texturePath.empty()) o.textureId=r.loadTexture(o.texturePath);
    if(!o.material.specularMapPath.empty()) o.material.specularMapId=r.loadTexture(o.material.specularMapPath);
    return o;
}

int pickObject(const Mat4& view, const Mat4& proj, float mouseX, float mouseY, float w, float h, const std::vector<SceneObject>& objs) { int best=-1; float minD=10000.0f; for(const auto& o:objs) { Vec4 wp(o.transform.position.x,o.transform.position.y,o.transform.position.z,1.0f); Vec4 cp=multiply(proj, multiply(view, wp)); if(cp.w<=0) continue; float sx=(cp.x/cp.w+1.0f)*0.5f*w; float sy=(1.0f-cp.y/cp.w)*0.5f*h; float d=std::sqrt(std::pow(sx-mouseX,2)+std::pow(sy-mouseY,2)); if(d<40.0f && cp.w<minD) { minD=cp.w; best=o.id; } } return best; }
void loadSceneFromFile(const std::string& path, std::vector<SceneObject>& objects, PrimitiveRenderer& renderer, Console& console, ProjectBrowser& browser) { std::ifstream f(path); if(!f.is_open()) return; try { json j; f>>j; if(!j.contains("objects")) return; objects.clear(); for(const auto& el:j["objects"]) objects.push_back(deserializeObject(el, renderer)); console.log("Loaded: "+path, LogType::Success); browser.navigateTo(path); } catch(...) { console.log("Load Failed", LogType::Error); } }

int main() {
    Window window(1600, 900, "DuckyEngine Editor");
    GuiLayer gui(window);
    Camera camera(Vec3(0.0f, 2.0f, 8.0f));
    Camera editorCamera = camera;
    PrimitiveRenderer renderer;
    ProjectBrowser browser(".");
    MenuBar menuBar;
    Viewport viewport(1000, 581);
    Console console;
    EditorSettings settings;

    std::vector<SceneObject> objects;
    int selectedId = -1;
    float deltaTime = 0.0f, lastFrame = 0.0f;

    SceneObject sun; sun.name="Sun"; sun.type=MeshType::Cube; sun.transform.position=Vec3(5,8,5); sun.transform.scale=Vec3(0.2f,0.2f,0.2f); sun.id=1; sun.hasCollider=false; objects.push_back(sun);
    SceneObject floor; floor.name="Floor"; floor.type=MeshType::Cube; floor.transform.position=Vec3(0,-2,0); floor.transform.scale=Vec3(10,0.1f,10); floor.id=2; floor.lockX=true; floor.lockY=true; floor.lockZ=true; objects.push_back(floor);
    SceneObject player; player.name="Player"; player.type=MeshType::Cube; player.transform.position=Vec3(0,2,0); player.id=3; player.useGravity=true; player.canShoot=true; objects.push_back(player);

    EngineMode currentMode = EngineMode::EDIT;
    EngineMode lastMode = EngineMode::EDIT;
    json sceneSnapshot;

    static int currentEffect = 3;
    const char* effects[] = { "Normal", "Invert", "Grayscale", "Cinematic", "Night Vision" };
    static ImGuizmo::OPERATION mCurrentGizmoOperation(ImGuizmo::TRANSLATE);
    static ImGuizmo::MODE mCurrentGizmoMode(ImGuizmo::WORLD);
    ImGuiWindowFlags windowFlags = ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize;

    glDisable(GL_CULL_FACE);
    glEnable(GL_DEPTH_TEST);

    while (!window.shouldClose()) {
        float currentFrame = (float)glfwGetTime(); deltaTime = currentFrame - lastFrame; lastFrame = currentFrame;
        window.pollEvents();

        // --- STATE MACHINE ---
        if (currentMode == EngineMode::PLAY && lastMode == EngineMode::EDIT) {
            console.log("Snapshot Saved.", LogType::Info);
            sceneSnapshot = json::array(); for (const auto& obj : objects) sceneSnapshot.push_back(serializeObject(obj));
            editorCamera = camera; selectedId = -1;
        }
        if (currentMode == EngineMode::EDIT && (lastMode == EngineMode::PLAY || lastMode == EngineMode::PAUSE)) {
            console.log("Restoring...", LogType::Warning);
            objects.clear();
            for (const auto& el : sceneSnapshot) objects.push_back(deserializeObject(el, renderer));
            camera = editorCamera;
            for(auto& obj : objects) {
                if (obj.vao == 0) {
                    if(obj.name.find("Sphere") != std::string::npos) { GeneratedMesh m = generateSphereMesh(32, 24); obj.vao = m.vao; obj.vertexCount = m.vertexCount; obj.type = MeshType::Model; }
                    else if(obj.name.find("Cylinder") != std::string::npos) { GeneratedMesh m = generateCylinderMesh(32); obj.vao = m.vao; obj.vertexCount = m.vertexCount; obj.type = MeshType::Model; }
                }
            }
        }
        lastMode = currentMode;

        if (currentMode == EngineMode::PLAY) {
            SceneObject* playerObj = nullptr; for(auto& obj : objects) if(obj.name == "Player") playerObj = &obj;
            if (playerObj) {
                float moveSpeed = 5.0f; playerObj->velocity.x = 0; playerObj->velocity.z = 0;
                if (glfwGetKey(window.getNativeWindow(), GLFW_KEY_UP) == GLFW_PRESS)    playerObj->velocity.z = -moveSpeed;
                if (glfwGetKey(window.getNativeWindow(), GLFW_KEY_DOWN) == GLFW_PRESS)  playerObj->velocity.z = moveSpeed;
                if (glfwGetKey(window.getNativeWindow(), GLFW_KEY_LEFT) == GLFW_PRESS)  playerObj->velocity.x = -moveSpeed;
                if (glfwGetKey(window.getNativeWindow(), GLFW_KEY_RIGHT) == GLFW_PRESS) playerObj->velocity.x = moveSpeed;
                if (glfwGetKey(window.getNativeWindow(), GLFW_KEY_SPACE) == GLFW_PRESS && std::abs(playerObj->velocity.y) < 0.01f) playerObj->velocity.y = 5.0f;
                camera.position = Vec3(playerObj->transform.position.x, playerObj->transform.position.y + 4.0f, playerObj->transform.position.z + 6.0f); camera.yaw = -90.0f; camera.pitch = -25.0f; camera.updateCameraVectors();
                if (playerObj->canShoot && glfwGetMouseButton(window.getNativeWindow(), 0) == GLFW_PRESS) {
                    int hit = shootRay(camera.position, camera.front, objects); if (hit != -1) { for(auto& o:objects) if(o.id==hit) { console.log("Hit: "+o.name, LogType::Warning); o.velocity.y = 5.0f; break; } }
                }
            }
            updatePhysics(objects, deltaTime);
        } else if (currentMode == EngineMode::EDIT) {
            if (glfwGetMouseButton(window.getNativeWindow(), 1) == GLFW_PRESS) {
                if (glfwGetKey(window.getNativeWindow(), GLFW_KEY_W) == GLFW_PRESS) camera.processKeyboard(FORWARD, deltaTime);
                if (glfwGetKey(window.getNativeWindow(), GLFW_KEY_S) == GLFW_PRESS) camera.processKeyboard(BACKWARD, deltaTime);
                if (glfwGetKey(window.getNativeWindow(), GLFW_KEY_A) == GLFW_PRESS) camera.processKeyboard(LEFT, deltaTime);
                if (glfwGetKey(window.getNativeWindow(), GLFW_KEY_D) == GLFW_PRESS) camera.processKeyboard(RIGHT, deltaTime);
                ImGuiIO& io = ImGui::GetIO(); camera.processMouseMovement(io.MouseDelta.x, -io.MouseDelta.y);
            }
            if (!ImGui::GetIO().WantCaptureKeyboard) {
                if (glfwGetKey(window.getNativeWindow(), GLFW_KEY_T) == GLFW_PRESS) mCurrentGizmoOperation = ImGuizmo::TRANSLATE;
                if (glfwGetKey(window.getNativeWindow(), GLFW_KEY_R) == GLFW_PRESS) mCurrentGizmoOperation = ImGuizmo::ROTATE;
                if (glfwGetKey(window.getNativeWindow(), GLFW_KEY_G) == GLFW_PRESS) mCurrentGizmoOperation = ImGuizmo::SCALE;
            }
        }

        for(auto& obj : objects) {
            if (obj.vao == 0) {
                if(obj.name.find("Sphere") != std::string::npos) { GeneratedMesh m = generateSphereMesh(32, 24); obj.vao = m.vao; obj.vertexCount = m.vertexCount; obj.type = MeshType::Model; }
                else if(obj.name.find("Cylinder") != std::string::npos) { GeneratedMesh m = generateCylinderMesh(32); obj.vao = m.vao; obj.vertexCount = m.vertexCount; obj.type = MeshType::Model; }
            }
        }

        Vec3 currentLightPos(2, 5, 2); for(auto& obj : objects) if(obj.name == "Sun") { currentLightPos = obj.transform.position; break; }
        renderer.drawShadows(objects, currentLightPos);
        viewport.bind(); glViewport(0, 0, 1000, 581); glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        Mat4 view = camera.getViewMatrix();
        Mat4 proj;
        if (settings.usePerspective) proj = MatrixTransform::perspective(camera.fov, 1000.0f / 581.0f, 0.1f, 100.0f);
        else proj = MatrixTransform::perspective(10.0f, 1000.0f / 581.0f, 0.1f, 100.0f);

        if (settings.isWireframe) glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        else glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

        renderer.drawSkybox(view, proj);
        if (settings.showGrid) renderer.drawGrid(view, proj);
        renderer.draw(objects, view, proj, camera.position, currentLightPos, selectedId);

        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        viewport.unbind(); viewport.drawPostProcess(currentEffect);

        gui.begin(); ImGuizmo::BeginFrame();
        menuBar.draw(window, objects, selectedId, browser, 1.0f / deltaTime, camera, renderer, console, currentMode, settings);

        float menuHeight = 25.0f; float toolbarHeight = 40.0f;
        float bottomHeight = (settings.showConsole || settings.showAssets) ? 300.0f : 0.0f;
        float mainAreaHeight = window.getHeight() - menuHeight - toolbarHeight - bottomHeight;
        if (mainAreaHeight < 100.0f) mainAreaHeight = 100.0f;

        ImGui::SetNextWindowPos(ImVec2(300, menuHeight)); ImGui::SetNextWindowSize(ImVec2(1000, toolbarHeight));
        ImGui::Begin("Toolbar", nullptr, ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoTitleBar);
        if (currentMode == EngineMode::EDIT) {
            ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.2f, 0.8f, 0.2f, 1.0f));
            if (ImGui::Button("PLAY", ImVec2(100, 25))) { currentMode = EngineMode::PLAY; } ImGui::PopStyleColor();
        } else {
            ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.8f, 0.2f, 0.2f, 1.0f));
            if (ImGui::Button("STOP", ImVec2(100, 25))) { currentMode = EngineMode::EDIT; } ImGui::PopStyleColor();
        }
        ImGui::End();

        if (settings.showHierarchy) {
            ImGui::SetNextWindowPos(ImVec2(0, menuHeight)); ImGui::SetNextWindowSize(ImVec2(300, mainAreaHeight + toolbarHeight));
            ImGui::Begin("Hierarchy", &settings.showHierarchy, windowFlags | ImGuiWindowFlags_NoTitleBar);
            for (auto& obj : objects) if (ImGui::Selectable(obj.name.c_str(), selectedId == obj.id)) selectedId = obj.id;
            ImGui::End();
        }

        ImGui::SetNextWindowPos(ImVec2(300, menuHeight + toolbarHeight)); ImGui::SetNextWindowSize(ImVec2(1000, mainAreaHeight));
        ImGui::Begin("Viewport", nullptr, windowFlags);
        ImVec2 vSize = ImGui::GetContentRegionAvail();
        if (ImGui::BeginDragDropTarget()) {
            if (const ImGuiPayload* p = ImGui::AcceptDragDropPayload("CONTENT_BROWSER_ITEM")) {
                std::string s((const char*)p->Data);
                if(s.find(".obj")!=std::string::npos) { SceneObject m=renderer.loadModel(s); if(m.vertexCount>0){ int max=0;for(auto&o:objects)if(o.id>max)max=o.id; m.id=max+1; m.hasCollider=true; m.useGravity=true; objects.push_back(m); } }
                else if(s.find(".ducky")!=std::string::npos) loadSceneFromFile(s, objects, renderer, console, browser);
            }
            ImGui::EndDragDropTarget();
        }
        if (currentMode == EngineMode::EDIT && ImGui::IsWindowHovered() && ImGui::IsMouseClicked(0) && !ImGuizmo::IsOver()) {
            ImVec2 mp = ImGui::GetMousePos(); ImVec2 wp = ImGui::GetWindowPos();
            int hit = pickObject(camera.getViewMatrix(), MatrixTransform::perspective(camera.fov, vSize.x/vSize.y, 0.1f, 100.f), mp.x - wp.x, mp.y - wp.y, vSize.x, vSize.y, objects);
            if (hit != -1) selectedId = hit;
        }
        ImGui::Image((void*)(intptr_t)viewport.getFinalTexture(), vSize, ImVec2(0, 1), ImVec2(1, 0));
        if(currentMode == EngineMode::PLAY) { ImVec2 center = ImVec2(ImGui::GetWindowPos().x + vSize.x/2, ImGui::GetWindowPos().y + vSize.y/2); ImGui::GetWindowDrawList()->AddCircleFilled(center, 3.0f, IM_COL32(255, 0, 0, 255)); }
        if (currentMode == EngineMode::EDIT && settings.showGizmos) {
            ImGuizmo::SetDrawlist(); ImVec2 wp = ImGui::GetWindowPos(); ImGuizmo::SetRect(wp.x, wp.y, vSize.x, vSize.y);
            if (selectedId != -1) {
                SceneObject* s = nullptr; for(auto& o : objects) if(o.id == selectedId) s = &o;
                if(s) {
                    float *v = (float*)view.data(), *p = (float*)proj.data(); Mat4 mm = s->transform.getModelMatrix(); float ma[16]; memcpy(ma, mm.data(), 64);
                    ImGuizmo::Manipulate(v, p, mCurrentGizmoOperation, mCurrentGizmoMode, ma);
                    if(ImGuizmo::IsUsing()) {
                        float t[3], r[3], sc[3];
                        ImGuizmo::DecomposeMatrixToComponents(ma, t, r, sc);
                        s->transform.position = Vec3(t[0], t[1], t[2]);
                        // --- FIX ROTACJI GIZMO (Konwersja na Radiany) ---
                        s->transform.rotation = Vec3(r[0] * DEG2RAD, r[1] * DEG2RAD, r[2] * DEG2RAD);
                        s->transform.scale = Vec3(sc[0], sc[1], sc[2]);
                    }
                }
            }
        }
        ImGui::End();


        if (settings.showInspector) {
            ImGui::SetNextWindowPos(ImVec2(1300, menuHeight)); ImGui::SetNextWindowSize(ImVec2(300, mainAreaHeight + toolbarHeight + bottomHeight));
            ImGui::Begin("Inspector", &settings.showInspector, windowFlags | ImGuiWindowFlags_NoTitleBar);
            ImGui::Combo("FX", &currentEffect, effects, IM_ARRAYSIZE(effects));
            ImGui::Separator();
            if (selectedId != -1 && currentMode == EngineMode::EDIT) {
                for (auto& obj : objects) {
                    if (obj.id == selectedId) {
                        char buf[128]; memset(buf,0,128); strncpy(buf, obj.name.c_str(), 127); if (ImGui::InputText("Name", buf, 128)) obj.name = std::string(buf);
                        if(obj.name=="Player") ImGui::TextColored(ImVec4(0,1,0,1),"Player Script Active");
                        if(ImGui::CollapsingHeader("Transform", ImGuiTreeNodeFlags_DefaultOpen)) {
                            ImGui::DragFloat3("Pos", &obj.transform.position.x, 0.1f);

                            // --- FIX ROTACJI INSPEKTORA (Wyświetlanie Stopni, zapis Radianów) ---
                            float rDeg[3] = { obj.transform.rotation.x * RAD2DEG, obj.transform.rotation.y * RAD2DEG, obj.transform.rotation.z * RAD2DEG };
                            if (ImGui::DragFloat3("Rot", rDeg, 0.5f)) {
                                obj.transform.rotation.x = rDeg[0] * DEG2RAD;
                                obj.transform.rotation.y = rDeg[1] * DEG2RAD;
                                obj.transform.rotation.z = rDeg[2] * DEG2RAD;
                            }

                            ImGui::DragFloat3("Scale", &obj.transform.scale.x, 0.05f);
                        }
                        if(ImGui::CollapsingHeader("Physics", ImGuiTreeNodeFlags_DefaultOpen)) { ImGui::Checkbox("Collider",&obj.hasCollider); ImGui::Checkbox("Gravity",&obj.useGravity); ImGui::Checkbox("Shoot",&obj.canShoot); ImGui::Checkbox("Lock Y",&obj.lockY); }

                        if(ImGui::CollapsingHeader("Material", ImGuiTreeNodeFlags_DefaultOpen)) {
                            ImGui::SliderFloat("Shininess", &obj.material.shininess, 1.0f, 256.0f);
                            ImGui::SliderFloat("Spec Strength", &obj.material.specularStrength, 0.0f, 2.0f);
                            ImGui::Spacing();
                            if(ImGui::Button("Diffuse", ImVec2(140, 0))) { const char* f = tinyfd_openFileDialog("Tex", "", 0, 0, 0, 0); if(f){obj.textureId=renderer.loadTexture(f); obj.texturePath=std::string(f);} }
                            ImGui::SameLine();
                            if(ImGui::Button("Specular", ImVec2(140, 0))) { const char* f = tinyfd_openFileDialog("Spec", "", 0, 0, 0, 0); if(f){obj.material.specularMapId=renderer.loadTexture(f); obj.material.specularMapPath=std::string(f);} }
                        }
                        ImGui::Spacing();
                        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.8f,0.2f,0.2f,1));
                        if(ImGui::Button("DELETE", ImVec2(-1, 0))) { for(auto it=objects.begin(); it!=objects.end(); ++it) if(it->id==selectedId){objects.erase(it);selectedId=-1;break;} }
                        ImGui::PopStyleColor();
                    }
                }
            }
            ImGui::End();
        }

        if (bottomHeight > 0) {
            ImGui::SetNextWindowPos(ImVec2(0, menuHeight + toolbarHeight + mainAreaHeight)); ImGui::SetNextWindowSize(ImVec2(1300, bottomHeight));
            ImGui::Begin("Bottom", nullptr, windowFlags | ImGuiWindowFlags_NoTitleBar);
            if (ImGui::BeginTabBar("T")) {
                if (settings.showAssets && ImGui::BeginTabItem("Project")) { std::string s = browser.draw(); if(!s.empty()) loadSceneFromFile(s, objects, renderer, console, browser); ImGui::EndTabItem(); }
                if (settings.showConsole && ImGui::BeginTabItem("Console")) { console.draw(); ImGui::EndTabItem(); }
                ImGui::EndTabBar();
            } ImGui::End();
        }

        gui.end(); window.swapBuffers();
    }
    return 0;
}