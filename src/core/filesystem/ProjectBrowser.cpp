#include "ProjectBrowser.hpp"
#include <imgui.h>
#include <iostream>

ProjectBrowser::ProjectBrowser(const std::string& rootPath) : currentDirectory(rootPath) {}

void ProjectBrowser::navigateTo(const std::string& filePath) {
    if (filePath.empty()) return;
    std::filesystem::path p(filePath);

    // Upewniamy się, że ścieżka istnieje
    if (std::filesystem::exists(p)) {
        // Jeśli to plik, bierzemy jego folder nadrzędny
        if (!std::filesystem::is_directory(p)) {
            currentDirectory = p.parent_path();
        } else {
            currentDirectory = p;
        }
    }
}

std::string ProjectBrowser::draw() {
    std::string sceneToLoad = "";

    // Pasek adresu (Tylko tekst, bez przycisku Back na górze)
    ImGui::Text("Current Dir: %s", currentDirectory.string().c_str());
    ImGui::Separator();

    float padding = 16.0f;
    float thumbnailSize = 64.0f;
    float cellSize = thumbnailSize + padding;
    float panelWidth = ImGui::GetContentRegionAvail().x;
    int columnCount = (int)(panelWidth / cellSize);
    if (columnCount < 1) columnCount = 1;

    if (ImGui::BeginTable("BrowserTable", columnCount)) {

        // --- PRZYCISK ".." (COFNIJ) JAKO PIERWSZA IKONA ---
        if (currentDirectory.has_parent_path()) {
            ImGui::TableNextColumn();
            ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.6f, 0.6f, 0.6f, 1.0f));
            if (ImGui::Button("..", ImVec2(thumbnailSize, thumbnailSize))) {
                currentDirectory = currentDirectory.parent_path();
            }
            ImGui::PopStyleColor();
            ImGui::TextWrapped("Back");
        }

        // --- PĘTLA PLIKÓW ---
        for (const auto& entry : std::filesystem::directory_iterator(currentDirectory)) {
            // Pomijamy pliki systemowe zaczynające się od kropki (np. .DS_Store na Mac)
            if (entry.path().filename().string()[0] == '.') continue;

            ImGui::TableNextColumn();

            std::string filename = entry.path().filename().string();
            std::string pathString = entry.path().string();
            bool isDir = entry.is_directory();

            ImGui::PushID(filename.c_str());

            // Kolory: Żółty dla folderów, Niebieski dla plików
            if (isDir) ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.8f, 0.7f, 0.2f, 1.0f));
            else       ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.2f, 0.4f, 0.8f, 1.0f));

            // Wyświetlamy przycisk (Ikona)
            ImGui::Button(isDir ? "[DIR]" : "[FILE]", ImVec2(thumbnailSize, thumbnailSize));
            ImGui::PopStyleColor();

            // Drag & Drop
            if (ImGui::BeginDragDropSource()) {
                ImGui::SetDragDropPayload("CONTENT_BROWSER_ITEM", pathString.c_str(), pathString.size() + 1);
                ImGui::Text("%s", filename.c_str());
                ImGui::EndDragDropSource();
            }

            // Obsługa kliknięcia (Wejście w folder lub ładowanie sceny)
            if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left)) {
                if (isDir) {
                    currentDirectory /= entry.path().filename();
                } else if (pathString.find(".ducky") != std::string::npos) {
                    sceneToLoad = pathString;
                }
            }

            ImGui::TextWrapped("%s", filename.c_str());
            ImGui::PopID();
        }
        ImGui::EndTable();
    }

    return sceneToLoad;
}