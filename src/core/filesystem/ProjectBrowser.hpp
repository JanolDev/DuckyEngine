#pragma once
#include <string>
#include <vector>
#include <filesystem>

class ProjectBrowser {
public:
    ProjectBrowser(const std::string& rootPath);

    std::string draw();


    void navigateTo(const std::string& filePath);

    std::string getDraggedFile() const { return draggedFile; }
    void clearDraggedFile() { draggedFile = ""; }

private:
    std::filesystem::path currentDirectory;
    std::string draggedFile;
};