cmake_minimum_required(VERSION 3.20)
project(DuckyEngine LANGUAGES CXX C)

add_definitions(-DIMGUI_DEFINE_MATH_OPERATORS)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Zapobiega problemom z duplikacją definicji na Macu
if(APPLE)
    set(CMAKE_MACOSX_RPATH 1)
endif()

# ────────────────────────────────────────────────────────────────
# 1. Znajdowanie zależności
# ────────────────────────────────────────────────────────────────
find_package(OpenGL REQUIRED)
find_package(glfw3 3.3 REQUIRED)

# ────────────────────────────────────────────────────────────────
# 2. Definicja grup plików (scalone i uporządkowane)
# ────────────────────────────────────────────────────────────────

# ImGui
set(IMGUI_DIR ${CMAKE_SOURCE_DIR}/external/imgui)
set(IMGUI_SOURCES
        ${IMGUI_DIR}/imgui.cpp
        ${IMGUI_DIR}/imgui_draw.cpp
        ${IMGUI_DIR}/imgui_tables.cpp
        ${IMGUI_DIR}/imgui_widgets.cpp
        ${IMGUI_DIR}/backends/imgui_impl_glfw.cpp
        ${IMGUI_DIR}/backends/imgui_impl_opengl3.cpp
)

# --- NOWOŚĆ: ImGuizmo ---
set(IMGUIZMO_DIR ${CMAKE_SOURCE_DIR}/ImGuizmo)
set(IMGUIZMO_SOURCES ${IMGUIZMO_DIR}/ImGuizmo.cpp)

# TinyFileDialogs
set(TINYFILEDIALOGS_DIR ${CMAKE_SOURCE_DIR}/external/tinyfiledialogs)
set(TINYFILEDIALOGS_SOURCES ${TINYFILEDIALOGS_DIR}/tinyfiledialogs.c)

# Matematyka i Core
set(ENGINE_SOURCES
        src/core/math/Mat4.cpp
        src/core/math/MatrixTransform.cpp
        src/core/camera/Camera.cpp
        src/core/renderer/Renderer.cpp
        src/core/viewport/Viewport.cpp
        src/core/sceneobject/SceneObject.cpp
        src/core/window/Window.cpp
        src/core/gui/GuiLayer.cpp
        src/core/filesystem/ProjectBrowser.cpp
        src/core/menubar/MenuBar.cpp
        src/core/textureloader/TextureLoader.cpp
        glad/src/glad.c
        src/core/gui/Console.cpp
        src/core/gui/Console.hpp
        src/core/math/Vec4.cpp
        src/core/math/Vec4.hpp
        src/core/renderer/tiny_obj_loader.h
        src/core/json.hpp
)

# ────────────────────────────────────────────────────────────────
# 3. Tworzenie pliku wykonywalnego
# ────────────────────────────────────────────────────────────────
add_executable(DuckyEngine
        main.cpp
        ${ENGINE_SOURCES}
        ${IMGUI_SOURCES}
        ${IMGUIZMO_SOURCES}       # Dodano źródła ImGuizmo
        ${TINYFILEDIALOGS_SOURCES}
)

# ────────────────────────────────────────────────────────────────
# 4. Include directories
# ────────────────────────────────────────────────────────────────
target_include_directories(DuckyEngine PRIVATE
        glad/include
        ${IMGUI_DIR}
        ${IMGUI_DIR}/backends
        ${IMGUIZMO_DIR}           # Dodano nagłówki ImGuizmo
        ${TINYFILEDIALOGS_DIR}
        src
        external/images           # Dla stb_image.h
)

# ────────────────────────────────────────────────────────────────
# 5. Linkowanie
# ────────────────────────────────────────────────────────────────
if(WIN32)
    target_link_libraries(DuckyEngine PRIVATE
            glfw
            OpenGL::GL
            opengl32
            gdi32
            user32
            shell32
    )
elseif(APPLE)
    target_link_libraries(DuckyEngine PRIVATE
            glfw
            OpenGL::GL
            "-framework Cocoa"
            "-framework IOKit"
            "-framework CoreVideo"
            "-framework QuartzCore"
    )
else()
    find_package(X11 REQUIRED)
    target_link_libraries(DuckyEngine PRIVATE
            glfw
            OpenGL::GL
            dl
            pthread
            X11::X11
    )
endif()

# ────────────────────────────────────────────────────────────────
# 6. Zasoby
# ────────────────────────────────────────────────────────────────
# (Tu ewentualnie Twoje komendy configure_file / file copy, jeśli masz)
file(COPY "${CMAKE_SOURCE_DIR}/assets" DESTINATION "${CMAKE_BINARY_DIR}")