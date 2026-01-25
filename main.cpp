#include <glad/glad.h>

#include "src/core/window/Window.hpp"
#include "src/core/gui/GuiLayer.hpp"
#include "src/core/filesystem/ProjectBrowser.hpp"
#include "src/core/menubar/MenuBar.hpp"
#include <tinyfiledialogs.h>
#include "src/core/textureloader/TextureLoader.hpp"
#include "src/core/renderer/Renderer.hpp"

int main() {
    bool showExplorer = true;

    // Define normal and splash screen window sizes
    const int NORMAL_WIDTH = 1280;
    const int NORMAL_HEIGHT = 800;
    const int SPLASH_WIDTH = 238;
    const int SPLASH_HEIGHT = 238;
    const float SPLASH_DURATION = 2.0f; // Duration of splash screen in seconds

    // Create window with initial splash screen size
    Window window(SPLASH_WIDTH, SPLASH_HEIGHT, "DuckyEngine Editor");
    GuiLayer gui(window);
    ProjectBrowser projectBrowser(".");
    MenuBar menuBar;
    TriangleRenderer triangle;

    // Load the application logo texture for the splash screen
    unsigned int logoTexture = loadTextureFromFile("../assets/icons/logo.png");
    unsigned int backgroundTexture = loadTextureFromFile("../assets/icons/background.png");

    // Define application states: SplashScreen or Running
    enum class AppState { SplashScreen, Running };
    AppState state = AppState::SplashScreen;
    float splashTimeElapsed = 0.0f;

    // ───────── MenuBar callbacks ─────────
    // Callback for creating a new project
    menuBar.onNewProject = [&]() {
        const char* folder = tinyfd_selectFolderDialog("Select Folder for New Project", ".");
        if (folder) projectBrowser.openFolder(folder);
    };

    // Callback for opening an existing project
    menuBar.onOpenProject = [&]() {
        const char* folder = tinyfd_selectFolderDialog("Select Existing Project", ".");
        if (folder) projectBrowser.openFolder(folder);
    };

    // Callback to exit the application
    menuBar.onExit = [&]() {
        window.closeWindow();
    };

    // Callback to undo folder navigation
    menuBar.onUndo = [&]() {
        if (!projectBrowser.m_historyBack.empty()) {
            projectBrowser.m_historyForward.push_back(projectBrowser.m_currentPath);
            projectBrowser.m_currentPath = projectBrowser.m_historyBack.back();
            projectBrowser.m_historyBack.pop_back();
        }
    };

    // Callback to redo folder navigation
    menuBar.onRedo = [&]() {
        if (!projectBrowser.m_historyForward.empty()) {
            projectBrowser.m_historyBack.push_back(projectBrowser.m_currentPath);
            projectBrowser.m_currentPath = projectBrowser.m_historyForward.back();
            projectBrowser.m_historyForward.pop_back();
        }
    };

    // Callback to toggle visibility of the Project Explorer panel
    menuBar.onToggleExplorer = [&]() {
        showExplorer = !showExplorer;
    };

    // ───────── Main loop ─────────
    while (!window.shouldClose()) {
        // Poll input events
        window.pollEvents();

        // Clear the screen with a dark gray background
        // glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        // glClear(GL_COLOR_BUFFER_BIT);
        window.setBackgroundColor(0.5f, 0.3f, 0.3f, 1.f);

        gui.begin();

        if (state == AppState::SplashScreen) {
            // Draw splash screen centered in the window
            gui.drawSplashScreen(logoTexture, SPLASH_DURATION);

            // Track elapsed time for splash screen
            splashTimeElapsed += ImGui::GetIO().DeltaTime;
            if (splashTimeElapsed >= SPLASH_DURATION) {
                state = AppState::Running; // Transition to main application
                window.setSize(NORMAL_WIDTH, NORMAL_HEIGHT); // Restore normal window size
            }

        } else if (state == AppState::Running) {
            // Draw menu bar and main application UI
            menuBar.draw();
            //draw ummm background... yeah, that is an excellent background image art... don't judge us. we just like duck... a very strong one... with muscles...
            gui.drawBackground(backgroundTexture, 0.2f);

            // Draw the Project Explorer if enabled
            if (showExplorer)
                projectBrowser.draw();
        }

        triangle.draw();



        gui.end();

        // Swap the front and back buffers to display the rendered frame
        window.swapBuffers();
    }

    return 0;
}
