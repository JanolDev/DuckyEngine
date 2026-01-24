#include "MenuBar.hpp"

void MenuBar::draw() {
    if (ImGui::BeginMainMenuBar()) {

        if (ImGui::BeginMenu("File")) {
            if (ImGui::MenuItem("New Project") && onNewProject) onNewProject();
            if (ImGui::MenuItem("Open Project") && onOpenProject) onOpenProject();
            if (ImGui::MenuItem("Exit") && onExit) onExit();
            ImGui::EndMenu();
        }

        if (ImGui::BeginMenu("Edit")) {
            if (ImGui::MenuItem("Undo") && onUndo) onUndo();
            if (ImGui::MenuItem("Redo") && onRedo) onRedo();
            ImGui::EndMenu();
        }

        if (ImGui::BeginMenu("View")) {
            if (ImGui::MenuItem("Toggle Explorer") && onToggleExplorer) onToggleExplorer();
            ImGui::EndMenu();
        }


        ImGui::EndMainMenuBar();
    }
}

