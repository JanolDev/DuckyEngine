#include "GuiLayer.hpp"
#include <iostream>
#include <filesystem>
#include <../../external/imgui/imgui.h>
#include <../../external/imgui/backends/imgui_impl_glfw.h>
#include <../../external/imgui/backends/imgui_impl_opengl3.h>

GuiLayer::GuiLayer(Window& window) {
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();

    ImGuiIO& io = ImGui::GetIO();


    setupTheme();


    const char* fontPath = "../assets/fonts/mainFont/JetBrainsMono-Bold.ttf";
    if (!std::filesystem::exists(fontPath) || !io.Fonts->AddFontFromFileTTF(fontPath, 16.0f)) {
        std::cout << "Font not found, using default fonts.\n";
        io.Fonts->AddFontDefault();
    }

    ImGui_ImplGlfw_InitForOpenGL(window.getNativeWindow(), true);
    ImGui_ImplOpenGL3_Init("#version 410");
}

GuiLayer::~GuiLayer() {
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
}

void GuiLayer::begin() {
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();
}

void GuiLayer::end() {
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void GuiLayer::setupTheme() {
    ImGuiStyle& style = ImGui::GetStyle();
    style.Colors[ImGuiCol_WindowBg]      = ImVec4(0.05f, 0.08f, 0.15f, 0.8f);
    style.Colors[ImGuiCol_ChildBg]       = ImVec4(0.07f, 0.10f, 0.18f, 0.8f);
    style.Colors[ImGuiCol_PopupBg]       = ImVec4(0.07f, 0.10f, 0.18f, 0.8f);
    style.Colors[ImGuiCol_Button]        = ImVec4(1.0f, 0.65f, 0.3f, 1.0f);
    style.Colors[ImGuiCol_ButtonHovered] = ImVec4(1.0f, 0.75f, 0.4f, 1.0f);
    style.Colors[ImGuiCol_ButtonActive]  = ImVec4(1.0f, 0.75f, 0.2f, 1.0f);
    style.Colors[ImGuiCol_Text]          = ImVec4(0.95f, 0.95f, 0.95f, 1.0f);
    style.Colors[ImGuiCol_Header]        = ImVec4(0.15f, 0.25f, 0.4f, 1.0f);
    style.Colors[ImGuiCol_HeaderHovered] = ImVec4(0.2f, 0.35f, 0.55f, 1.0f);
    style.Colors[ImGuiCol_HeaderActive]  = ImVec4(0.1f, 0.2f, 0.35f, 1.0f);
    style.WindowRounding = 5.0f;
    style.FrameRounding = 3.0f;
    style.WindowPadding = ImVec2(10, 10);
    style.ItemSpacing = ImVec2(8, 6);
}

void GuiLayer::drawSplashScreen(unsigned int textureID, float durationSeconds) {
    static float timeElapsed = 0.0f;
    timeElapsed += ImGui::GetIO().DeltaTime;
    if (timeElapsed > durationSeconds) return;

    ImGuiIO& io = ImGui::GetIO();
    ImVec2 windowPos(io.DisplaySize.x / 2 - 128, io.DisplaySize.y / 2 - 128);
    ImVec2 windowSize(256, 256);

    ImGui::SetNextWindowPos(windowPos);
    ImGui::SetNextWindowSize(windowSize);

    ImGuiWindowFlags flags = ImGuiWindowFlags_NoTitleBar |
                             ImGuiWindowFlags_NoResize |
                             ImGuiWindowFlags_NoMove |
                             ImGuiWindowFlags_NoCollapse |
                             ImGuiWindowFlags_NoBackground |
                             ImGuiWindowFlags_NoScrollbar;

    ImGui::Begin("SplashScreen", nullptr, flags);
    ImGui::Image((void*)(intptr_t)textureID, ImVec2(256, 256));
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
