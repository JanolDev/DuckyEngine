#include "ProjectBrowser.hpp"
#include <imgui.h>

ProjectBrowser::ProjectBrowser(const std::string& rootPath) : currentPath(rootPath) {}

void ProjectBrowser::draw() {
    float cellSize = 85.0f;
    float padding = 15.0f;
    float panelWidth = ImGui::GetContentRegionAvail().x;
    int columnCount = (int)(panelWidth / (cellSize + padding));
    if (columnCount < 1) columnCount = 1;

    ImGui::Columns(columnCount, 0, false);

    for (const auto& entry : std::filesystem::directory_iterator(currentPath)) {
        std::string filename = entry.path().filename().string();
        ImGui::PushID(filename.c_str());

        // Rozróżnienie kolorami Folderów i Plików
        if (entry.is_directory()) {
            ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.85f, 0.65f, 0.2f, 1.0f));
            if (ImGui::Button("DIR", ImVec2(cellSize, cellSize))) currentPath = entry.path();
            ImGui::PopStyleColor();
        } else {
            ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.3f, 0.3f, 0.35f, 1.0f));
            ImGui::Button("FILE", ImVec2(cellSize, cellSize));
            ImGui::PopStyleColor();
        }

        ImGui::TextWrapped("%s", filename.c_str());
        ImGui::NextColumn();
        ImGui::PopID();
    }
    ImGui::Columns(1);
}

void ProjectBrowser::setPath(const std::string& path) { currentPath = path; }