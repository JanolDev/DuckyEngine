#pragma once

#include <string>
#include <filesystem>
#include <vector>


class ProjectBrowser {
public:
    explicit ProjectBrowser(const std::string& rootPath);

    void draw();
    void openFolder(const std::string& path);

    std::filesystem::path m_currentPath;
    std::filesystem::path m_selectedPath;
    std::vector<std::filesystem::path> m_historyBack;
    std::vector<std::filesystem::path> m_historyForward;


    void drawDirectory(const std::filesystem::path& path);

    void drawToolbar();
    void createFolder();
    void createFile();
    void deleteSelected();
    void renameSelected();
private:
    bool m_renaming = false;
    std::filesystem::path m_renamingPath;
    char m_renameBuffer[256] = {0};

};
