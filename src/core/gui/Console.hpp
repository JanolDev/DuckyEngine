#pragma once
#include <vector>
#include <string>
#include <imgui.h>

enum class LogType {
    Info,
    Warning,
    Error,
    Success
};

struct LogEntry {
    std::string message;
    LogType type;
    float time;
};

class Console {
public:
    Console();
    void log(const std::string& message, LogType type = LogType::Info);
    void draw();
    void clear();

private:
    std::vector<LogEntry> m_logs;
    bool m_autoScroll;
    bool m_scrollToBottom;
};