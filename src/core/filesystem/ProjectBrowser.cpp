#include "ProjectBrowser.hpp"
#include <fstream>
#include <filesystem>
#include <algorithm>
#include <../../external/imgui/imgui.h>
#include <../../external/tinyfiledialogs/tinyfiledialogs.h>

// Constructor: initialize the current path to the absolute path of the given root folder
ProjectBrowser::ProjectBrowser(const std::string& rootPath) {
    m_currentPath = std::filesystem::absolute(rootPath);
}

// Draw the main project explorer window
void ProjectBrowser::draw() {
    ImGuiIO& io = ImGui::GetIO();

    // Set window position and size to stick at the bottom of the application
    ImVec2 windowPos(0, io.DisplaySize.y - 300);
    ImVec2 windowSize(io.DisplaySize.x, 300);
    ImGui::SetNextWindowPos(windowPos);
    ImGui::SetNextWindowSize(windowSize);

    // Window flags: immovable, unresizable, non-collapsible
    ImGuiWindowFlags windowFlags = ImGuiWindowFlags_NoMove |
                                   ImGuiWindowFlags_NoResize |
                                   ImGuiWindowFlags_NoCollapse;

    // Begin window
    ImGui::Begin("Project Explorer", nullptr, windowFlags);

    // Draw navigation buttons
    if (ImGui::Button("<- Back") && !m_historyBack.empty()) {
        m_historyForward.push_back(m_currentPath);
        m_currentPath = m_historyBack.back();
        m_historyBack.pop_back();
    }

    ImGui::SameLine();
    if (ImGui::Button("Open New Repository")) {
        const char* folder = tinyfd_selectFolderDialog("Select Project Folder", m_currentPath.string().c_str());
        if (folder) {
            m_historyBack.push_back(m_currentPath);
            m_currentPath = std::filesystem::path(folder);
            m_historyForward.clear();
        }
    }

    ImGui::SameLine();
    if (ImGui::Button("Forward ->") && !m_historyForward.empty()) {
        m_historyBack.push_back(m_currentPath);
        m_currentPath = m_historyForward.back();
        m_historyForward.pop_back();
    }

    // Display current folder path
    ImGui::TextWrapped("%s", m_currentPath.string().c_str());

    // Draw toolbar for actions like create/delete/refresh
    drawToolbar();

    // Draw the directory contents
    drawDirectory(m_currentPath);

    ImGui::End();
}

void ProjectBrowser::renameSelected() {
    if (!m_selectedPath.empty()) {
        m_renamingPath = m_selectedPath;
        strcpy(m_renameBuffer, m_selectedPath.filename().string().c_str());
    }
}

// Draw all files and folders in the given path
void ProjectBrowser::drawDirectory(const std::filesystem::path& path) {
    for (const auto& entry : std::filesystem::directory_iterator(path)) {
        const auto& p = entry.path();
        const std::string name = p.filename().string();

        bool isSelected = (m_selectedPath == p);

        // If this item is currently being renamed
        if (p == m_renamingPath) {
            ImGui::SetKeyboardFocusHere();
            if (ImGui::InputText("##rename", m_renameBuffer, sizeof(m_renameBuffer), ImGuiInputTextFlags_EnterReturnsTrue)) {
                std::filesystem::path newPath = p.parent_path() / m_renameBuffer;
                if (!std::filesystem::exists(newPath)) {
                    std::filesystem::rename(p, newPath);
                    m_selectedPath = newPath;
                }
                m_renamingPath.clear(); // stop renaming
            }
        } else {
            if (entry.is_directory()) {
                if (ImGui::Selectable((name + "/").c_str(), isSelected)) {
                    m_selectedPath = p;
                }

                if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(0)) {
                    m_historyBack.push_back(m_currentPath);
                    m_currentPath = p;
                    m_historyForward.clear();
                }
            } else {
                if (ImGui::Selectable(name.c_str(), isSelected)) {
                    m_selectedPath = p;
                }
            }
        }
    }
}

// Open a folder programmatically and update history
void ProjectBrowser::openFolder(const std::string& path) {
    m_historyBack.push_back(m_currentPath);
    m_currentPath = std::filesystem::path(path);
    m_historyForward.clear();
}

// Draw toolbar with create/delete/refresh buttons
void ProjectBrowser::drawToolbar() {
    if (ImGui::SmallButton("Create Folder")) {
        createFolder();
    }
    ImGui::SameLine();

    if (ImGui::SmallButton("Create File")) {
        createFile();
    }
    ImGui::SameLine();

    if (ImGui::SmallButton("Delete File/Folder")) {
        deleteSelected();
    }
    ImGui::SameLine();

    if (ImGui::SmallButton("Refresh")) {
        refresh();
    }
    ImGui::SameLine();
    if (ImGui::SmallButton("Rename")) {
        renameSelected();
    }


    ImGui::Separator();
}

// Create a new folder in the current directory with automatic naming
void ProjectBrowser::createFolder() {
    std::filesystem::path newDir = m_currentPath / "NewFolder";
    int i = 1;
    while (std::filesystem::exists(newDir)) {
        newDir = m_currentPath / ("NewFolder" + std::to_string(i++));
    }
    std::filesystem::create_directory(newDir);
}

// Create a new file in the current directory with automatic naming
void ProjectBrowser::createFile() {
    std::filesystem::path newFile = m_currentPath / "NewFile.txt";
    int i = 1;
    while (std::filesystem::exists(newFile)) {
        newFile = m_currentPath / ("NewFile" + std::to_string(i++) + ".txt");
    }
    std::ofstream(newFile.string());
}

// Delete the currently selected file or folder
void ProjectBrowser::deleteSelected() {
    if (m_selectedPath.empty())
        return;

    std::filesystem::remove_all(m_selectedPath);

    // Remove deleted path from history stacks
    auto removeFromHistory = [&](auto& history) {
        history.erase(std::remove(history.begin(), history.end(), m_selectedPath), history.end());
    };
    removeFromHistory(m_historyBack);
    removeFromHistory(m_historyForward);

    // Clear selection and cancel renaming if necessary
    if (m_renamingPath == m_selectedPath)
        m_renamingPath.clear();

    m_selectedPath.clear();
}

// Refresh the explorer (currently filesystem auto-refreshes)
void ProjectBrowser::refresh() {
    // Nothing needed: std::filesystem always reflects current state
}
