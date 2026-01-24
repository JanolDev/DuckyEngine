#include "ProjectBrowser.hpp"
#include <../../external/imgui/imgui.h>
#include <filesystem>
#include <../../external/tinyfiledialogs/tinyfiledialogs.h>

ProjectBrowser::ProjectBrowser(const std::string& rootPath) {
    // Initialize the current path as the absolute path of the provided root path
    m_currentPath = std::filesystem::absolute(rootPath);
}

void ProjectBrowser::draw() {
    // Get ImGui IO to access display size and other input/output settings
    ImGuiIO& io = ImGui::GetIO();

    // Set the window position and size to stick to the bottom of the screen
    ImVec2 windowPos(0, io.DisplaySize.y - 300); // y-position offset from top
    ImVec2 windowSize(io.DisplaySize.x, 300);     // full width, 300px height
    ImGui::SetNextWindowPos(windowPos);
    ImGui::SetNextWindowSize(windowSize);

    // Define window flags to make it immovable, unresizable, and non-collapsible
    ImGuiWindowFlags windowFlags = ImGuiWindowFlags_NoMove |
                                   ImGuiWindowFlags_NoResize |
                                   ImGuiWindowFlags_NoCollapse;

    // Begin the ImGui window with specified flags
    ImGui::Begin("Project Explorer", nullptr, windowFlags);

    // Draw "Back" button and navigate backward in history if possible
    if (ImGui::Button("<- Back") && !m_historyBack.empty()) {
        m_historyForward.push_back(m_currentPath);
        m_currentPath = m_historyBack.back();
        m_historyBack.pop_back();
    }

    // Place the next button on the same line
    ImGui::SameLine();

    // Draw "Open New Repository" button to select a folder using TinyFileDialogs
    if (ImGui::Button("Open New Repository")) {
        const char* folder = tinyfd_selectFolderDialog("Select Project Folder", m_currentPath.string().c_str());
        if (folder) {
            m_historyBack.push_back(m_currentPath); // Save current path to back history
            m_currentPath = std::filesystem::path(folder); // Update to selected folder
            m_historyForward.clear(); // Clear forward history after navigation
        }
    }

    // Place the next button on the same line
    ImGui::SameLine();

    // Draw "Forward" button and navigate forward in history if possible
    if (ImGui::Button("Forward ->") && !m_historyForward.empty()) {
        m_historyBack.push_back(m_currentPath);
        m_currentPath = m_historyForward.back();
        m_historyForward.pop_back();
    }

    // Display the current folder path, wrapped if too long
    ImGui::TextWrapped("%s", m_currentPath.string().c_str());

    // Draw the directory contents of the current path
    drawDirectory(m_currentPath);

    // End the ImGui window
    ImGui::End();
}

void ProjectBrowser::drawDirectory(const std::filesystem::path& path) {
    // Iterate through each entry in the directory
    for (const auto& entry : std::filesystem::directory_iterator(path)) {
        if (entry.is_directory()) {
            // Create a collapsible tree node for directories
            if (ImGui::TreeNode(entry.path().filename().string().c_str())) {
                // Provide an "Open" selectable to navigate into the directory
                if (ImGui::Selectable("Open")) {
                    m_historyBack.push_back(m_currentPath); // Save current path to history
                    m_currentPath = entry.path();           // Navigate into the directory
                    m_historyForward.clear();               // Clear forward history
                }
                // Close the tree node for this directory
                ImGui::TreePop();
            }
        } else {
            // Display file name as a bullet point for regular files
            ImGui::BulletText("%s", entry.path().filename().string().c_str());
        }
    }
}

void ProjectBrowser::openFolder(const std::string& path) {
    // Directly open a folder and update history
    m_historyBack.push_back(m_currentPath); // Save current path to back history
    m_currentPath = std::filesystem::path(path); // Update to the new folder path
    m_historyForward.clear(); // Clear forward history after navigation
}
