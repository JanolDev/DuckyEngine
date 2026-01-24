#pragma once
#include <imgui.h>
#include <functional>


class MenuBar {
public:
    std::function<void()> onNewProject = nullptr;
    std::function<void()> onOpenProject = nullptr;
    std::function<void()> onExit = nullptr;

    std::function<void()> onUndo = nullptr;
    std::function<void()> onRedo = nullptr;

    std::function<void()> onToggleExplorer = nullptr;

    MenuBar() = default;
    ~MenuBar() = default;

    void draw();
};

