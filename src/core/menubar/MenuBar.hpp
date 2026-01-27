#pragma once
#include <vector>
#include "../window/Window.hpp"
#include "../sceneobject/SceneObject.hpp"
#include "../filesystem/ProjectBrowser.hpp"
#include "../camera/Camera.hpp"
#include "../gui/Console.hpp"

class PrimitiveRenderer;

// Tryby silnika
enum class EngineMode { EDIT, PLAY, PAUSE };

// --- STRUKTURA USTAWIEŃ EDYTORA ---
// To tutaj trzymamy stan wszystkich "ptaszków" z menu
struct EditorSettings {
    // View
    bool usePerspective = true;
    bool showGrid = true;
    bool showGizmos = true;
    bool isWireframe = false;

    // Window toggles
    bool showScene = true;
    bool showGame = true;
    bool showHierarchy = true;
    bool showInspector = true;
    bool showAssets = true;
    bool showConsole = true;
    bool showProfiler = false; // Placeholder

    // Tools
    bool debugView = false;
};

class MenuBar {
public:
    void draw(Window& window,
              std::vector<SceneObject>& objects,
              int& selectedId,
              ProjectBrowser& browser,
              float fps,
              Camera& camera,
              PrimitiveRenderer& renderer,
              Console& console,
              EngineMode& currentMode,
              EditorSettings& settings); // <--- Przekazujemy ustawienia

private:
    // Undo/Redo
    std::vector<std::vector<SceneObject>> undoStack;
    std::vector<std::vector<SceneObject>> redoStack;
    void saveState(const std::vector<SceneObject>& currentObjects);
    void performUndo(std::vector<SceneObject>& currentObjects, int& selectedId);
    void performRedo(std::vector<SceneObject>& currentObjects, int& selectedId);

    // Stan pełnego ekranu
    bool isFullscreen = false;
    int winX=0, winY=0, winW=0, winH=0;

    // Popupy
    bool showAboutModal = false;
    bool showShortcutsModal = false;
    bool showPreferencesModal = false;
};