#pragma once
#include <string>
#include <filesystem>

class ProjectBrowser {
public:
    ProjectBrowser(const std::string& rootPath);
    void draw();
    void setPath(const std::string& newPath);
    std::string getCurrentPath() const;

private:
    std::filesystem::path currentPath;
};