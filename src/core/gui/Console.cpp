#include "Console.hpp"
#include <GLFW/glfw3.h> // Do pobrania czasu

Console::Console() : m_autoScroll(true), m_scrollToBottom(false) {
    log("DuckyEngine Console initialized.", LogType::Success);
}

void Console::clear() {
    m_logs.clear();
}

void Console::log(const std::string& message, LogType type) {
    LogEntry entry;
    entry.message = message;
    entry.type = type;
    entry.time = (float)glfwGetTime();
    m_logs.push_back(entry);
    
    if (m_autoScroll) m_scrollToBottom = true;
}

void Console::draw() {
    // Pasek narzędzi konsoli
    if (ImGui::Button("Clear")) clear();
    ImGui::SameLine();
    ImGui::Checkbox("Auto-scroll", &m_autoScroll);
    ImGui::Separator();

    // Obszar logów
    ImGui::BeginChild("ScrollingRegion", ImVec2(0, 0), false, ImGuiWindowFlags_HorizontalScrollbar);

    for (const auto& entry : m_logs) {
        // Kolorowanie w zależności od typu
        ImVec4 color;
        std::string prefix;
        
        switch (entry.type) {
            case LogType::Error:   color = ImVec4(1.0f, 0.4f, 0.4f, 1.0f); prefix = "[ERROR] "; break;
            case LogType::Warning: color = ImVec4(1.0f, 1.0f, 0.4f, 1.0f); prefix = "[WARN]  "; break;
            case LogType::Success: color = ImVec4(0.4f, 1.0f, 0.4f, 1.0f); prefix = "[OK]    "; break;
            default:               color = ImVec4(0.9f, 0.9f, 0.9f, 1.0f); prefix = "[INFO]  "; break;
        }

        ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.5f, 0.5f, 0.5f, 1.0f));
        ImGui::Text("[%.1fs] ", entry.time);
        ImGui::PopStyleColor();
        ImGui::SameLine();

        ImGui::PushStyleColor(ImGuiCol_Text, color);
        ImGui::TextUnformatted((prefix + entry.message).c_str());
        ImGui::PopStyleColor();
    }

    // Auto-scrollowanie
    if (m_scrollToBottom) {
        ImGui::SetScrollHereY(1.0f);
        m_scrollToBottom = false;
    }

    ImGui::EndChild();
}