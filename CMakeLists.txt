cmake_minimum_required(VERSION 3.20)
project(DuckyEngine LANGUAGES CXX C)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# ────────────────────────────────────────────────────────────────
# Znajdowanie zależności
# ────────────────────────────────────────────────────────────────
find_package(OpenGL REQUIRED)
find_package(glfw3 3.3 REQUIRED)

# ────────────────────────────────────────────────────────────────
# ImGui
# ────────────────────────────────────────────────────────────────
set(IMGUI_DIR ${CMAKE_SOURCE_DIR}/external/imgui)
set(IMGUI_SOURCES
        ${IMGUI_DIR}/imgui.cpp
        ${IMGUI_DIR}/imgui_draw.cpp
        ${IMGUI_DIR}/imgui_tables.cpp
        ${IMGUI_DIR}/imgui_widgets.cpp
        ${IMGUI_DIR}/backends/imgui_impl_glfw.cpp
        ${IMGUI_DIR}/backends/imgui_impl_opengl3.cpp
)

# ────────────────────────────────────────────────────────────────
# tinyfiledialogs
# ────────────────────────────────────────────────────────────────
set(TINYFILEDIALOGS_DIR ${CMAKE_SOURCE_DIR}/external/tinyfiledialogs)
set(TINYFILEDIALOGS_SOURCES
        ${TINYFILEDIALOGS_DIR}/tinyfiledialogs.c
        src/core/menubar/menubar.cpp
        src/core/menubar/menubar.hpp
        src/core/textureloader/TextureLoader.cpp
        src/core/textureloader/TextureLoader.hpp
        external/images/stb_image.h
        src/core/renderer/Renderer.cpp
        src/core/renderer/Renderer.hpp
)
# ────────── Fonts ──────────
file(GLOB FONT_FILES "${CMAKE_SOURCE_DIR}/ui/font/mainFont/*")
foreach(FONT_FILE ${FONT_FILES})
    configure_file(${FONT_FILE} ${CMAKE_BINARY_DIR}/font/mainFont COPYONLY)
endforeach()

# ────────── Icons ──────────
file(GLOB ICON_FILES "${CMAKE_SOURCE_DIR}/ui/icon/*")
foreach(ICON_FILE ${ICON_FILES})
    configure_file(${ICON_FILE} ${CMAKE_BINARY_DIR}/ui/icon COPYONLY)
endforeach()

# ────────────────────────────────────────────────────────────────
# Źródła projektu
# ────────────────────────────────────────────────────────────────
add_executable(DuckyEngine
        main.cpp
        glad/src/glad.c
        src/core/window/Window.cpp
        src/core/gui/GuiLayer.cpp
        src/core/filesystem/ProjectBrowser.cpp
        src/core/menubar/menubar.cpp
        src/core/menubar/menubar.hpp
        src/core/textureloader/TextureLoader.cpp
        src/core/textureloader/TextureLoader.hpp
        src/core/renderer/Renderer.cpp
        src/core/renderer/Renderer.hpp
        ${IMGUI_SOURCES}
        ${TINYFILEDIALOGS_SOURCES}
)

# ────────────────────────────────────────────────────────────────
# Include directories
# ────────────────────────────────────────────────────────────────
target_include_directories(DuckyEngine PRIVATE
        glad/include
        ${IMGUI_DIR}
        ${IMGUI_DIR}/backends
        ${TINYFILEDIALOGS_DIR}
        src
        ${STB_DIR}
)

# ────────────────────────────────────────────────────────────────
# Linkowanie – różne na platformach
# ────────────────────────────────────────────────────────────────
if(WIN32) #Windows
    target_link_libraries(DuckyEngine PRIVATE
            glfw
            OpenGL::GL
            opengl32
            gdi32
            user32
            shell32
    )
elseif(APPLE) #Macbook
    target_link_libraries(DuckyEngine PRIVATE
            glfw
            OpenGL::GL
            "-framework Cocoa"
            "-framework IOKit"
            "-framework CoreVideo"
    )
else() # Linux
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
# Flagi kompilacji / definicje
# ────────────────────────────────────────────────────────────────
target_compile_definitions(DuckyEngine PRIVATE
        GL_MAJOR_VERSION=4
        GL_MINOR_VERSION=1
)
