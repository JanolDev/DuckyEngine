#include "GuiLayer.hpp"
#include <iostream>
#include <filesystem>

#include <../../external/imgui/imgui.h>
#include <../../external/imgui/backends/imgui_impl_glfw.h>
#include <../../external/imgui/backends/imgui_impl_opengl3.h>

GuiLayer::GuiLayer(Window& window) {
    // Verify ImGui version and create a new ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();

    // Apply custom color theme and styling
    setupTheme();

    // Access ImGui IO structure for fonts and display settings
    ImGuiIO& io = ImGui::GetIO();

    // Load a custom fonts, fallback to default if not found
    const char* fontPath = "../assets/fonts/mainFont/JetBrainsMono-Bold.ttf";
    if (!std::filesystem::exists(fontPath) || !io.Fonts->AddFontFromFileTTF(fontPath, 16.0f)) {
        std::cout << "Font not found, using default fonts.\n";
        io.Fonts->AddFontDefault();
    }

    // Initialize ImGui GLFW backend for input handling
    ImGui_ImplGlfw_InitForOpenGL(window.getHandle(), true);

    // Initialize ImGui OpenGL3 backend for rendering
    ImGui_ImplOpenGL3_Init("#version 410");
}

GuiLayer::~GuiLayer() {
    // Shutdown ImGui OpenGL3 and GLFW backends and destroy context
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
}

void GuiLayer::begin() {
    // Start a new ImGui frame for both OpenGL3 and GLFW backends
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();
}

void GuiLayer::end() {
    // Render ImGui draw data using OpenGL3 backend
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void GuiLayer::setupTheme() {
    // Access ImGui style object to customize colors, rounding, and spacing
    ImGuiStyle& style = ImGui::GetStyle();

    // Set background colors for windows, child windows, and popups
    style.Colors[ImGuiCol_WindowBg]      = ImVec4(0.05f, 0.08f, 0.15f, 0.8f);
    style.Colors[ImGuiCol_ChildBg]       = ImVec4(0.07f, 0.10f, 0.18f, 0.8f);
    style.Colors[ImGuiCol_PopupBg]       = ImVec4(0.07f, 0.10f, 0.18f, 0.8f);

    // Customize button colors for default, hovered, and active states
    style.Colors[ImGuiCol_Button]        = ImVec4(1.0f, 0.65f, 0.3f, 1.0f);
    style.Colors[ImGuiCol_ButtonHovered] = ImVec4(1.0f, 0.75f, 0.4f, 1.0f);
    style.Colors[ImGuiCol_ButtonActive]  = ImVec4(1.0f, 0.75f, 0.2f, 1.0f);

    // Set text color
    style.Colors[ImGuiCol_Text]          = ImVec4(0.95f, 0.95f, 0.95f, 1.0f);

    // Set header colors for collapsible elements like folders
    style.Colors[ImGuiCol_Header]        = ImVec4(0.15f, 0.25f, 0.4f, 1.0f);
    style.Colors[ImGuiCol_HeaderHovered] = ImVec4(0.2f, 0.35f, 0.55f, 1.0f);
    style.Colors[ImGuiCol_HeaderActive]  = ImVec4(0.1f, 0.2f, 0.35f, 1.0f);

    // Set rounding, padding, and spacing for windows and items
    style.WindowRounding = 5.0f;
    style.FrameRounding = 3.0f;
    style.WindowPadding = ImVec2(10, 10);
    style.ItemSpacing = ImVec2(8, 6);
}

void GuiLayer::drawSplashScreen(unsigned int textureID, float durationSeconds) {
    // Static variable to track elapsed time for splash screen display
    static float timeElapsed = 0.0f;
    timeElapsed += ImGui::GetIO().DeltaTime;

    // Skip rendering if splash duration has passed
    if (timeElapsed > durationSeconds)
        return;

    // Calculate window position to center the splash screen
    ImGuiIO& io = ImGui::GetIO();
    ImVec2 windowPos(io.DisplaySize.x / 2 - 128, io.DisplaySize.y / 2 - 128);
    ImVec2 windowSize(256, 256);

    // Set next window position and size
    ImGui::SetNextWindowPos(windowPos);
    ImGui::SetNextWindowSize(windowSize);

    // Configure window flags to make splash screen immovable, unresizable, and backgroundless
    ImGuiWindowFlags flags = ImGuiWindowFlags_NoTitleBar |
                             ImGuiWindowFlags_NoResize |
                             ImGuiWindowFlags_NoMove |
                             ImGuiWindowFlags_NoCollapse |
                             ImGuiWindowFlags_NoBackground |
                             ImGuiWindowFlags_NoScrollbar;

    // Begin ImGui window for splash screen
    ImGui::Begin("SplashScreen", nullptr, flags);

    // Render the texture as an image
    ImGui::Image((void*)(intptr_t)textureID, ImVec2(256, 256));

    // End splash screen window
    ImGui::End();
}
void GuiLayer::drawBackground(unsigned int textureID, float alpha) {
    ImGuiIO& io = ImGui::GetIO();

    ImDrawList* drawList = ImGui::GetBackgroundDrawList();

    drawList->AddImage(
        (void*)(intptr_t)textureID,
        ImVec2(0, 0),
        ImVec2(io.DisplaySize.x, io.DisplaySize.y),
        ImVec2(0, 0),
        ImVec2(1, 1),
        IM_COL32(255, 255, 255, static_cast<int>(255 * alpha))
    );
}
