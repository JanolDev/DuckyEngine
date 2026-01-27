#pragma once
#include <vector>
#include <string>
#include "../window/Window.hpp"
#include "../sceneobject/SceneObject.hpp"
#include "../filesystem/ProjectBrowser.hpp"
#include "../camera/Camera.hpp"

class MenuBar {
public:
    bool showAbout = false;
    void draw(Window& window, std::vector<SceneObject>& sceneObjects, int& selectedId, ProjectBrowser& browser, float fps, Camera& camera);

private:
    void saveProject(const std::vector<SceneObject>& objects, const std::string& path);
    void loadProject(std::vector<SceneObject>& objects, const std::string& path);
};