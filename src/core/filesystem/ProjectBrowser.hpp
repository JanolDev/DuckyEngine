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
    std::vector<std::filesystem::path> m_historyBack;
    std::vector<std::filesystem::path> m_historyForward;

    void drawDirectory(const std::filesystem::path& path);
};
