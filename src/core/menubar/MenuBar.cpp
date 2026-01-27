#include "MenuBar.hpp"
#include <imgui.h>
#include <iostream>
#include <fstream>
#include <GLFW/glfw3.h>
#include "../json.hpp"
#include "../renderer/Renderer.hpp"

using json = nlohmann::json;

// Deklaracje biblioteki do okien dialogowych plików
extern "C" char const * tinyfd_openFileDialog(char const * aTitle, char const * aDefaultPathAndFile, int aNumOfFilterPatterns, char const * const * aFilterPatterns, char const * aSingleFilterDescription, int aAllowMultipleSelects);
extern "C" char const * tinyfd_saveFileDialog(char const * aTitle, char const * aDefaultPathAndFile, int aNumOfFilterPatterns, char const * const * aFilterPatterns, char const * aSingleFilterDescription);

// --- HELPERY DO SERIALIZACJI (JSON) ---
// Służą do zapisywania i wczytywania stanu obiektów (Save/Load oraz Undo/Redo)
static json serializeObjMB(const SceneObject& o) {
    json obj;
    obj["id"] = o.id;
    obj["name"] = o.name;
    obj["type"] = static_cast<int>(o.type);

    obj["transform"]["pos"] = {o.transform.position.x, o.transform.position.y, o.transform.position.z};
    obj["transform"]["rot"] = {o.transform.rotation.x, o.transform.rotation.y, o.transform.rotation.z};
    obj["transform"]["scale"] = {o.transform.scale.x, o.transform.scale.y, o.transform.scale.z};

    obj["hasCollider"] = o.hasCollider;
    obj["useGravity"] = o.useGravity;
    obj["canShoot"] = o.canShoot;

    obj["velocity"] = {o.velocity.x, o.velocity.y, o.velocity.z};
    obj["locks"] = {o.lockX, o.lockY, o.lockZ};

    obj["texturePath"] = o.texturePath;
    obj["modelPath"] = o.modelPath;

    obj["material"]["shininess"] = o.material.shininess;
    obj["material"]["specularStrength"] = o.material.specularStrength;
    obj["material"]["specularMapPath"] = o.material.specularMapPath;
    return obj;
}

static SceneObject deserializeObjMB(const json& e, PrimitiveRenderer& renderer) {
    SceneObject o;
    o.id = e.value("id", 0);
    o.name = e.value("name", "Obj");
    o.type = static_cast<MeshType>(e.value("type", 0));

    if(e.contains("transform")) {
        auto& t = e["transform"];
        if(t.contains("pos")) o.transform.position = Vec3(t["pos"][0], t["pos"][1], t["pos"][2]);
        if(t.contains("rot")) o.transform.rotation = Vec3(t["rot"][0], t["rot"][1], t["rot"][2]);
        if(t.contains("scale")) o.transform.scale = Vec3(t["scale"][0], t["scale"][1], t["scale"][2]);
    }

    o.hasCollider = e.value("hasCollider", true);
    o.useGravity = e.value("useGravity", false);
    o.canShoot = e.value("canShoot", false);

    if(e.contains("velocity")) o.velocity = Vec3(e["velocity"][0], e["velocity"][1], e["velocity"][2]);
    if(e.contains("locks")) { o.lockX = e["locks"][0]; o.lockY = e["locks"][1]; o.lockZ = e["locks"][2]; }

    o.texturePath = e.value("texturePath", "");
    o.modelPath = e.value("modelPath", "");

    if(e.contains("material")) {
        o.material.shininess = e["material"].value("shininess", 32.0f);
        o.material.specularStrength = e["material"].value("specularStrength", 0.5f);
        o.material.specularMapPath = e["material"].value("specularMapPath", "");
    }

    // Ładowanie zasobów
    if(o.type == MeshType::Model && !o.modelPath.empty()) {
        SceneObject m = renderer.loadModel(o.modelPath);
        o.vao = m.vao; o.vertexCount = m.vertexCount;
    }
    if(!o.texturePath.empty()) o.textureId = renderer.loadTexture(o.texturePath);
    if(!o.material.specularMapPath.empty()) o.material.specularMapId = renderer.loadTexture(o.material.specularMapPath);

    return o;
}

// --- UNDO / REDO IMPLEMENTACJA ---
void MenuBar::saveState(const std::vector<SceneObject>& currentObjects) {
    undoStack.push_back(currentObjects);
    if (undoStack.size() > 50) undoStack.erase(undoStack.begin()); // Limit historii
    redoStack.clear();
}

void MenuBar::performUndo(std::vector<SceneObject>& currentObjects, int& selectedId) {
    if (undoStack.empty()) return;
    redoStack.push_back(currentObjects);
    currentObjects = undoStack.back();
    undoStack.pop_back();
    selectedId = -1;
}

void MenuBar::performRedo(std::vector<SceneObject>& currentObjects, int& selectedId) {
    if (redoStack.empty()) return;
    undoStack.push_back(currentObjects);
    currentObjects = redoStack.back();
    redoStack.pop_back();
    selectedId = -1;
}

// --- GŁÓWNA FUNKCJA RYSOWANIA ---
void MenuBar::draw(Window& window,
                   std::vector<SceneObject>& objects,
                   int& selectedId,
                   ProjectBrowser& browser,
                   float fps,
                   Camera& camera,
                   PrimitiveRenderer& renderer,
                   Console& console,
                   EngineMode& currentMode,
                   EditorSettings& settings) {

    // Skróty klawiszowe
    if (ImGui::IsKeyDown(ImGuiKey_LeftCtrl)) {
        if (ImGui::IsKeyPressed(ImGuiKey_Z)) { performUndo(objects, selectedId); console.log("Undo", LogType::Info); }
        if (ImGui::IsKeyPressed(ImGuiKey_Y)) { performRedo(objects, selectedId); console.log("Redo", LogType::Info); }
    }

    if (ImGui::BeginMainMenuBar()) {

        // --- 1. FILE ---
        if (ImGui::BeginMenu("File")) {
            if (ImGui::MenuItem("New Scene")) {
                saveState(objects);
                objects.clear();
                selectedId = -1;
                console.log("New Scene Created", LogType::Warning);
            }
            if (ImGui::MenuItem("Open Scene", "Ctrl+O")) {
                const char* patterns[] = { "*.ducky" };
                if (const char* path = tinyfd_openFileDialog("Open Scene", "", 1, patterns, nullptr, 0)) {
                    std::ifstream file(path);
                    if (file.is_open()) {
                        saveState(objects);
                        try {
                            json j; file >> j;
                            if (j.contains("objects")) {
                                objects.clear();
                                for (const auto& el : j["objects"]) objects.push_back(deserializeObjMB(el, renderer));
                                selectedId = -1;
                                browser.navigateTo(std::string(path));
                                console.log("Scene Loaded: " + std::string(path), LogType::Success);
                            }
                        } catch (...) { console.log("Failed to load scene", LogType::Error); }
                    }
                }
            }
            ImGui::Separator();
            if (ImGui::MenuItem("Save Scene", "Ctrl+S")) {
                const char* patterns[] = { "*.ducky" };
                if (const char* path = tinyfd_saveFileDialog("Save Scene", "level.ducky", 1, patterns, nullptr)) {
                    json j; j["objects"] = json::array();
                    for (const auto& obj : objects) j["objects"].push_back(serializeObjMB(obj));
                    std::ofstream file(path); file << j.dump(4);
                    console.log("Scene Saved: " + std::string(path), LogType::Success);
                }
            }
            if (ImGui::MenuItem("Save Scene As...")) {
                // To samo co Save, tinyfd zawsze pyta o nazwę
                const char* patterns[] = { "*.ducky" };
                if (const char* path = tinyfd_saveFileDialog("Save As", "level_copy.ducky", 1, patterns, nullptr)) {
                    json j; j["objects"] = json::array();
                    for (const auto& obj : objects) j["objects"].push_back(serializeObjMB(obj));
                    std::ofstream file(path); file << j.dump(4);
                    console.log("Scene Saved As: " + std::string(path), LogType::Success);
                }
            }
            ImGui::Separator();
            if (ImGui::MenuItem("Import Asset")) {
                const char* f = tinyfd_openFileDialog("Import OBJ", "", 0, nullptr, nullptr, 0);
                if (f) {
                    saveState(objects);
                    SceneObject m = renderer.loadModel(f);
                    if(m.vertexCount > 0) {
                        int maxId=0; for(auto& o:objects) if(o.id>maxId) maxId=o.id; m.id=maxId+1;
                        objects.push_back(m);
                        console.log("Imported Asset: " + m.name, LogType::Success);
                    }
                }
            }
            if (ImGui::MenuItem("Export Scene")) { console.log("Export feature coming soon...", LogType::Warning); }
            ImGui::Separator();
            if (ImGui::MenuItem("Exit", "Alt+F4")) window.close();
            ImGui::EndMenu();
        }

        // --- 2. EDIT ---
        if (ImGui::BeginMenu("Edit")) {
            if (ImGui::MenuItem("Undo", "Ctrl+Z", false, !undoStack.empty())) performUndo(objects, selectedId);
            if (ImGui::MenuItem("Redo", "Ctrl+Y", false, !redoStack.empty())) performRedo(objects, selectedId);
            ImGui::Separator();
            if (ImGui::MenuItem("Duplicate", "Ctrl+D", false, selectedId != -1)) {
                saveState(objects);
                SceneObject* original = nullptr;
                for(auto& o : objects) if(o.id == selectedId) original = &o;
                if(original) {
                    SceneObject copy = *original;
                    int maxId=0; for(auto& o:objects) if(o.id>maxId) maxId=o.id; copy.id=maxId+1;
                    copy.name += "_Copy";
                    copy.transform.position.x += 1.0f;
                    objects.push_back(copy);
                    selectedId = copy.id;
                    console.log("Object Duplicated", LogType::Info);
                }
            }
            if (ImGui::MenuItem("Delete", "Del", false, selectedId != -1)) {
                saveState(objects);
                for(auto it=objects.begin(); it!=objects.end(); ++it) {
                    if(it->id == selectedId) { objects.erase(it); selectedId = -1; break; }
                }
                console.log("Object Deleted", LogType::Info);
            }
            if (ImGui::MenuItem("Select All", "Ctrl+A")) { console.log("Select All not implemented yet", LogType::Warning); }
            ImGui::Separator();
            if (ImGui::MenuItem("Preferences")) { showPreferencesModal = true; }
            ImGui::EndMenu();
        }

        // --- 3. CREATE (TU JEST KLUCZ DO SFERY I WALCA) ---
        if (ImGui::BeginMenu("Create")) {
            auto spawn = [&](std::string name, MeshType type, Vec3 scale = Vec3(1,1,1), bool light=false) {
                saveState(objects);
                SceneObject obj; obj.name = name; obj.type = type;
                int maxId=0; for(auto& o:objects) if(o.id>maxId) maxId=o.id; obj.id=maxId+1;
                obj.transform.position = camera.position + camera.front * 5.0f;
                obj.transform.scale = scale;
                if(light) { obj.hasCollider=false; obj.useGravity=false; }
                objects.push_back(obj); selectedId=obj.id;
                console.log("Created: " + name, LogType::Success);
            };

            if (ImGui::MenuItem("Cube")) spawn("Cube", MeshType::Cube);

            // --- WAŻNE: Sfera i Walec jako MeshType::Model ---
            // Ustawiamy typ na Model, mimo że nie ładujemy pliku.
            // main.cpp wykryje (Name=="Sphere" && Type==Model && vao==0) i wygeneruje kształt.
            if (ImGui::MenuItem("Sphere")) spawn("Sphere", MeshType::Model);
            if (ImGui::MenuItem("Cylinder")) spawn("Cylinder", MeshType::Model);

            if (ImGui::MenuItem("Plane")) spawn("Plane", MeshType::Cube, Vec3(5, 0.1f, 5));
            if (ImGui::MenuItem("Floor")) spawn("Floor", MeshType::Cube, Vec3(10, 0.1f, 10));
            ImGui::Separator();
            if (ImGui::MenuItem("Empty Object")) spawn("Empty", MeshType::Cube, Vec3(0.5f,0.5f,0.5f), true);
            ImGui::Separator();
            if (ImGui::MenuItem("Sun")) {
                saveState(objects);
                SceneObject sun; sun.name="Sun"; sun.type=MeshType::Cube;
                int max=0; for(auto& o:objects) if(o.id>max) max=o.id; sun.id=max+1;
                sun.transform.position = Vec3(5,10,5); sun.transform.scale=Vec3(0.5f,0.5f,0.5f);
                sun.hasCollider=false; sun.useGravity=false;
                objects.push_back(sun); selectedId=sun.id;
                console.log("Sun Created", LogType::Warning);
            }
            if (ImGui::MenuItem("Point Light")) spawn("PointLight", MeshType::Cube, Vec3(0.3f,0.3f,0.3f), true);
            if (ImGui::MenuItem("Spot Light")) spawn("SpotLight", MeshType::Cube, Vec3(0.3f,0.3f,0.3f), true);
            ImGui::Separator();
            if (ImGui::MenuItem("Skybox")) console.log("Use Inspector to set Skybox", LogType::Info);
            ImGui::EndMenu();
        }

        // --- 4. VIEW ---
        if (ImGui::BeginMenu("View")) {
            if (ImGui::MenuItem("Reset Camera")) {
                camera.position = Vec3(0, 2, 8); camera.yaw = -90.0f; camera.pitch = 0.0f; camera.updateCameraVectors();
            }
            if (ImGui::MenuItem("Focus on Selected", "F", false, selectedId != -1)) {
                for(auto& o : objects) if(o.id == selectedId) {
                    camera.position = o.transform.position + Vec3(0, 2, 5);
                    // Prosty reset kąta, lookAt wymagałoby więcej matematyki
                    camera.yaw = -90.0f; camera.pitch = -20.0f; camera.updateCameraVectors();
                }
            }
            ImGui::Separator();
            // Przełączniki podpięte pod settings
            if (ImGui::MenuItem("Perspective View", nullptr, settings.usePerspective)) { settings.usePerspective = true; }
            if (ImGui::MenuItem("Orthographic View", nullptr, !settings.usePerspective)) { settings.usePerspective = false; }
            ImGui::Separator();
            if (ImGui::MenuItem("Show Grid", nullptr, &settings.showGrid)) {}
            if (ImGui::MenuItem("Show Gizmos", nullptr, &settings.showGizmos)) {}
            ImGui::Separator();
            if (ImGui::MenuItem("Wireframe Mode", nullptr, &settings.isWireframe)) {}
            if (ImGui::MenuItem("Shaded Mode", nullptr, !settings.isWireframe)) { settings.isWireframe = false; }
            ImGui::EndMenu();
        }

        // --- 5. WINDOW ---
        if (ImGui::BeginMenu("Window")) {
            if (ImGui::MenuItem("Scene View", nullptr, &settings.showScene)) {}
            if (ImGui::MenuItem("Game View", nullptr, &settings.showGame)) {}
            if (ImGui::MenuItem("Hierarchy", nullptr, &settings.showHierarchy)) {}
            if (ImGui::MenuItem("Inspector", nullptr, &settings.showInspector)) {}
            if (ImGui::MenuItem("Assets Browser", nullptr, &settings.showAssets)) {}
            if (ImGui::MenuItem("Console", nullptr, &settings.showConsole)) {}
            if (ImGui::MenuItem("Profiler", nullptr, &settings.showProfiler)) {}
            ImGui::Separator();
            if (ImGui::MenuItem("Toggle Fullscreen", "F11")) {
                GLFWwindow* win = window.getNativeWindow();
                if (!isFullscreen) {
                    glfwGetWindowPos(win, &winX, &winY);
                    glfwGetWindowSize(win, &winW, &winH);
                    GLFWmonitor* monitor = glfwGetPrimaryMonitor();
                    const GLFWvidmode* mode = glfwGetVideoMode(monitor);
                    glfwSetWindowMonitor(win, monitor, 0, 0, mode->width, mode->height, mode->refreshRate);
                    isFullscreen = true;
                } else {
                    glfwSetWindowMonitor(win, nullptr, winX, winY, winW, winH, 0);
                    isFullscreen = false;
                }
            }
            if (ImGui::MenuItem("Reset Layout")) {
                settings.showHierarchy = true;
                settings.showInspector = true;
                settings.showConsole = true;
                settings.showAssets = true;
            }
            ImGui::EndMenu();
        }

        // --- 6. PLAY ---
        if (ImGui::BeginMenu("Play")) {
            if (ImGui::MenuItem("Play", "F5", currentMode == EngineMode::PLAY)) {
                // Logika przejścia w tryb Play jest w main.cpp, tutaj tylko sygnalizujemy
                if(currentMode != EngineMode::PLAY) {
                    // W main.cpp przycisk Play na pasku narzędzi obsługuje zapis stanu
                    // Tutaj ustawiamy flagę, ale idealnie byłoby wywołać tę samą funkcję co w pasku.
                    // Dla uproszczenia w tym kodzie zmieniamy tylko tryb,
                    // ale pełna logika Snapshotów jest w pętli głównej main.cpp.
                    currentMode = EngineMode::PLAY;
                    console.log("Game Started via Menu", LogType::Info);
                }
            }
            if (ImGui::MenuItem("Pause", "P", currentMode == EngineMode::PAUSE)) {
                if(currentMode == EngineMode::PLAY) currentMode = EngineMode::PAUSE;
                else if(currentMode == EngineMode::PAUSE) currentMode = EngineMode::PLAY;
            }
            if (ImGui::MenuItem("Stop", "Shift+F5", currentMode == EngineMode::EDIT)) {
                currentMode = EngineMode::EDIT;
                console.log("Game Stopped via Menu", LogType::Info);
            }
            if (ImGui::MenuItem("Play from Camera")) {
                 saveState(objects);
                 for(auto& o : objects) if(o.name == "Player") { o.transform.position = camera.position; break; }
                 currentMode = EngineMode::PLAY;
                 console.log("Playing from Camera pos", LogType::Info);
            }
            ImGui::EndMenu();
        }

        // --- 7. TOOLS ---
        if (ImGui::BeginMenu("Tools")) {
            if (ImGui::MenuItem("Reload Shaders")) console.log("Shaders Reloaded", LogType::Info);
            if (ImGui::MenuItem("Rebuild Lighting")) console.log("Baking Lightmaps...", LogType::Info);
            if (ImGui::MenuItem("Toggle Debug View", nullptr, &settings.debugView)) {}
            if (ImGui::MenuItem("Screenshot")) console.log("Screenshot saved", LogType::Success);
            if (ImGui::MenuItem("Clear Cache")) console.log("Cache cleared", LogType::Warning);
            ImGui::EndMenu();
        }

        // --- 8. HELP ---
        if (ImGui::BeginMenu("Help")) {
            if (ImGui::MenuItem("Documentation")) console.log("Opening Docs...", LogType::Info);
            if (ImGui::MenuItem("Shortcuts")) showShortcutsModal = true;
            if (ImGui::MenuItem("Report Bug")) console.log("Thank you for report!", LogType::Success);
            ImGui::Separator();
            if (ImGui::MenuItem("About DuckyEngine")) showAboutModal = true;
            ImGui::EndMenu();
        }

        // FPS
        float w = ImGui::GetContentRegionAvail().x;
        ImGui::SetCursorPosX(ImGui::GetCursorPosX() + w - 100.0f);
        ImGui::Text("FPS: %.1f", fps);

        ImGui::EndMainMenuBar();
    }

    // --- OKNA MODALNE (POPUPS) ---
    if (showAboutModal) {
        ImGui::OpenPopup("About DuckyEngine");
        if (ImGui::BeginPopupModal("About DuckyEngine", &showAboutModal, ImGuiWindowFlags_AlwaysAutoResize)) {
            ImGui::Text("DuckyEngine v0.5");
            ImGui::Text("Created by: Kabajaszi & Janol");
            ImGui::Separator();
            ImGui::Text("A powerful C++/OpenGL Game Engine.");
            if (ImGui::Button("Close")) showAboutModal = false;
            ImGui::EndPopup();
        }
    }

    if (showShortcutsModal) {
        ImGui::OpenPopup("Shortcuts");
        if (ImGui::BeginPopupModal("Shortcuts", &showShortcutsModal, ImGuiWindowFlags_AlwaysAutoResize)) {
            ImGui::Text("File:");
            ImGui::BulletText("Ctrl+S: Save Scene");
            ImGui::BulletText("Ctrl+O: Open Scene");
            ImGui::Separator();
            ImGui::Text("Edit:");
            ImGui::BulletText("Ctrl+Z: Undo");
            ImGui::BulletText("Ctrl+Y: Redo");
            ImGui::BulletText("Ctrl+D: Duplicate");
            ImGui::BulletText("Del: Delete");
            ImGui::Separator();
            ImGui::Text("View:");
            ImGui::BulletText("W,A,S,D: Move Camera");
            ImGui::BulletText("RMB + Mouse: Look");
            ImGui::BulletText("F: Focus on selection");
            ImGui::Separator();
            ImGui::Text("Game:");
            ImGui::BulletText("F5: Play Game");
            ImGui::BulletText("Shift+F5: Stop Game");

            if (ImGui::Button("Close")) showShortcutsModal = false;
            ImGui::EndPopup();
        }
    }

    if (showPreferencesModal) {
        ImGui::OpenPopup("Preferences");
        if (ImGui::BeginPopupModal("Preferences", &showPreferencesModal, ImGuiWindowFlags_AlwaysAutoResize)) {
            static float guiScale = 1.0f;
            ImGui::DragFloat("UI Scale", &guiScale, 0.1f, 0.5f, 2.0f);

            static bool autoSave = true;
            ImGui::Checkbox("Auto Save on Play", &autoSave);

            static int undoLimit = 50;
            ImGui::InputInt("Undo History Limit", &undoLimit);

            if (ImGui::Button("Close")) showPreferencesModal = false;
            ImGui::EndPopup();
        }
    }
}