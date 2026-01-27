#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <imgui.h>
#include "src/core/window/Window.hpp"
#include "src/core/gui/GuiLayer.hpp"
#include "src/core/menubar/MenuBar.hpp"
#include "src/core/camera/Camera.hpp"
#include "src/core/renderer/Renderer.hpp"
#include "src/core/viewport/Viewport.hpp"

struct EngineLog {
    std::vector<std::string> items;
    void add(std::string msg) { items.push_back("[System] " + msg); }
    void draw() {
        for (auto& item : items) ImGui::TextUnformatted(item.c_str());
        if (ImGui::GetScrollY() >= ImGui::GetScrollMaxY()) ImGui::SetScrollHereY(1.0f);
    }
};

void processInput(GLFWwindow* window, Camera& camera, float dt) {
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) camera.processKeyboard(FORWARD, dt);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) camera.processKeyboard(BACKWARD, dt);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) camera.processKeyboard(LEFT, dt);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) camera.processKeyboard(RIGHT, dt);
}

int main() {
    Window window(1600, 900, "DuckyEngine v1.5");
    GuiLayer gui(window);
    Camera camera;
    PrimitiveRenderer renderer;
    ProjectBrowser browser(".");
    MenuBar menuBar;
    Viewport viewport(1000, 580);
    EngineLog logger;
    logger.add("DuckyEngine v1.5 initialized.");

    std::vector<SceneObject> objects;
    int selectedId = -1;
    float deltaTime = 0.0f, lastFrame = 0.0f;

    ImGuiWindowFlags windowFlags = ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize;

    while (!window.shouldClose()) {
        float currentFrame = (float)glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        window.pollEvents();
        if (glfwGetMouseButton(window.getNativeWindow(), 1) == GLFW_PRESS)
            processInput(window.getNativeWindow(), camera, deltaTime);

        gui.begin();
        // Przekazujemy kamerę do menubar
        menuBar.draw(window, objects, selectedId, browser, 1.0f / deltaTime, camera);

        // HIERARCHY
        ImGui::SetNextWindowPos(ImVec2(0, 19));
        ImGui::SetNextWindowSize(ImVec2(300, 581));
        ImGui::Begin("Hierarchy", nullptr, windowFlags);
        for (auto& obj : objects) {
            if (ImGui::Selectable(obj.name.c_str(), selectedId == obj.id)) selectedId = obj.id;
        }
        ImGui::End();

        // VIEWPORT
        ImGui::SetNextWindowPos(ImVec2(300, 19));
        ImGui::SetNextWindowSize(ImVec2(1000, 581));
        ImGui::Begin("Viewport", nullptr, windowFlags);
        ImVec2 vSize = ImGui::GetContentRegionAvail();
        if (ImGui::IsWindowHovered() && ImGui::IsMouseDown(1)) {
            ImGuiIO& io = ImGui::GetIO();
            camera.processMouseMovement(io.MouseDelta.x, -io.MouseDelta.y);
        }
        viewport.bind();
        glViewport(0, 0, (int)vSize.x, (int)vSize.y);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        renderer.draw(objects, camera.getViewMatrix(), MatrixTransform::perspective(45.0f, vSize.x/vSize.y, 0.1f, 100.0f), selectedId);
        viewport.unbind();
        ImGui::Image((void*)(intptr_t)viewport.getTexture(), vSize, ImVec2(0, 1), ImVec2(1, 0));
        ImGui::End();

        // INSPECTOR
        ImGui::SetNextWindowPos(ImVec2(1300, 19));
        ImGui::SetNextWindowSize(ImVec2(300, 881));
        ImGui::Begin("Inspector", nullptr, windowFlags);
        if (selectedId != -1) {
            for (auto& obj : objects) {
                if (obj.id == selectedId) {
                    ImGui::TextColored(ImVec4(0.4f, 1.0f, 0.4f, 1.0f), "Object: %s", obj.name.c_str());
                    ImGui::Separator();
                    if (ImGui::CollapsingHeader("Transform", ImGuiTreeNodeFlags_DefaultOpen)) {
                        ImGui::DragFloat3("Position", &obj.transform.position.x, 0.1f);
                        ImGui::DragFloat3("Rotation", &obj.transform.rotation.x, 1.0f);
                        ImGui::DragFloat3("Scale", &obj.transform.scale.x, 0.1f);
                    }
                }
            }
        } else {
            ImGui::Text("Select an object in Hierarchy.");
        }
        ImGui::End();

        // BOTTOM PANEL
        ImGui::SetNextWindowPos(ImVec2(0, 600));
        ImGui::SetNextWindowSize(ImVec2(1300, 300));
        ImGui::Begin("AssetsPanel", nullptr, windowFlags | ImGuiWindowFlags_NoTitleBar);
        if (ImGui::BeginTabBar("BottomTabs")) {
            if (ImGui::BeginTabItem("Project")) { browser.draw(); ImGui::EndTabItem(); }
            if (ImGui::BeginTabItem("Console")) { logger.draw(); ImGui::EndTabItem(); }
            ImGui::EndTabBar();
        }
        ImGui::End();

        gui.end();
        window.swapBuffers();
    }
    return 0;
}