#include "MenuBar.hpp"
#include <imgui.h>
#include <fstream>
#include <sstream>
#include <filesystem>

extern "C" char const * tinyfd_saveFileDialog(char const * aTitle, char const * aDefaultPathAndFile, int aNumOfFilterPatterns, char const * const * aFilterPatterns, char const * aSingleFilterDescription);
extern "C" char const * tinyfd_openFileDialog(char const * aTitle, char const * aDefaultPathAndFile, int aNumOfFilterPatterns, char const * const * aFilterPatterns, char const * aSingleFilterDescription, int aAllowMultipleSelects);

void MenuBar::draw(Window& window, std::vector<SceneObject>& sceneObjects, int& selectedId, ProjectBrowser& browser, float fps, Camera& camera) {
    if (ImGui::BeginMainMenuBar()) {
        if (ImGui::BeginMenu("File")) {
            if (ImGui::MenuItem("New Scene")) { sceneObjects.clear(); selectedId = -1; }
            if (ImGui::MenuItem("Save Scene As...")) {
                const char* filter[] = { "*.ducky" };
                const char* path = tinyfd_saveFileDialog("Save", "scene.ducky", 1, filter, "Ducky Scene");
                if (path) {
                    saveProject(sceneObjects, path);
                    browser.setPath(std::filesystem::path(path).parent_path().string()); // Synchronizacja folderu
                }
            }
            if (ImGui::MenuItem("Load Scene...")) {
                const char* filter[] = { "*.ducky" };
                const char* path = tinyfd_openFileDialog("Open", "", 1, filter, "Ducky Scene", 0);
                if (path) {
                    loadProject(sceneObjects, path);
                    browser.setPath(std::filesystem::path(path).parent_path().string()); // Synchronizacja folderu
                }
            }
            ImGui::Separator();
            if (ImGui::MenuItem("Exit")) window.close();
            ImGui::EndMenu();
        }

        if (ImGui::BeginMenu("Entity")) {
            if (ImGui::BeginMenu("3D Primitives")) {
                if (ImGui::MenuItem("Cube")) {
                    SceneObject obj; obj.id = (int)sceneObjects.size(); obj.name = "Cube_" + std::to_string(obj.id);
                    obj.type = MeshType::Cube; sceneObjects.push_back(obj);
                }
                if (ImGui::MenuItem("Pyramid")) {
                    SceneObject obj; obj.id = (int)sceneObjects.size(); obj.name = "Pyramid_" + std::to_string(obj.id);
                    obj.type = MeshType::Pyramid; sceneObjects.push_back(obj);
                }
                if (ImGui::MenuItem("Triangle")) {
                    SceneObject obj; obj.id = (int)sceneObjects.size(); obj.name = "Triangle_" + std::to_string(obj.id);
                    obj.type = MeshType::Triangle; sceneObjects.push_back(obj);
                }
                ImGui::EndMenu();
            }
            ImGui::EndMenu();
        }

        if (ImGui::BeginMenu("View")) { // Nowe menu
            if (ImGui::MenuItem("Reset Camera")) {
                camera.reset();
            }
            ImGui::EndMenu();
        }

        if (ImGui::BeginMenu("Help")) {
            if (ImGui::MenuItem("About DuckyEngine")) { showAbout = true; }
            ImGui::EndMenu();
        }

        float statsX = ImGui::GetWindowWidth() - 280.0f;
        ImGui::SameLine(statsX);
        ImGui::TextColored(ImVec4(0.4f, 0.8f, 1.0f, 1.0f), "FPS: %.0f", fps);
        ImGui::SameLine();
        ImGui::Text("| Objects: %zu", sceneObjects.size());

        ImGui::EndMainMenuBar();
    }

    if (showAbout) {
        ImGui::OpenPopup("About DuckyEngine");
        if (ImGui::BeginPopupModal("About DuckyEngine", &showAbout, ImGuiWindowFlags_AlwaysAutoResize)) {
            ImGui::Spacing();
            ImGui::TextColored(ImVec4(1.0f, 0.8f, 0.0f, 1.0f), "DUCKY ENGINE v0.3");
            ImGui::Separator();
            ImGui::Text("A modern OpenGL Engine for 2D, 3D Development.");
            ImGui::Text("Developed by: Kabajaszi & JanolDev");
            ImGui::Spacing();
            ImGui::Separator();
            if (ImGui::Button("Close", ImVec2(120, 0))) { showAbout = false; ImGui::CloseCurrentPopup(); }
            ImGui::EndPopup();
        }
    }
}

void MenuBar::saveProject(const std::vector<SceneObject>& objects, const std::string& path) {
    std::ofstream out(path);
    for (const auto& obj : objects) {
        out << obj.id << "|" << obj.name << "|" << (int)obj.type << "|"
            << obj.transform.position.x << "|" << obj.transform.position.y << "|" << obj.transform.position.z << "|"
            << obj.transform.rotation.x << "|" << obj.transform.rotation.y << "|" << obj.transform.rotation.z << "|"
            << obj.transform.scale.x << "|" << obj.transform.scale.y << "|" << obj.transform.scale.z << "\n";
    }
}

void MenuBar::loadProject(std::vector<SceneObject>& objects, const std::string& path) {
    std::ifstream in(path); if (!in) return;
    objects.clear(); std::string line;
    while (std::getline(in, line)) {
        std::stringstream ss(line); std::string part; std::vector<std::string> v;
        while (std::getline(ss, part, '|')) v.push_back(part);
        if (v.size() >= 12) {
            SceneObject obj;
            obj.id = std::stoi(v[0]); obj.name = v[1]; obj.type = (MeshType)std::stoi(v[2]);
            obj.transform.position = {std::stof(v[3]), std::stof(v[4]), std::stof(v[5])};
            obj.transform.rotation = {std::stof(v[6]), std::stof(v[7]), std::stof(v[8])};
            obj.transform.scale = {std::stof(v[9]), std::stof(v[10]), std::stof(v[11])};
            objects.push_back(obj);
        }
    }
}