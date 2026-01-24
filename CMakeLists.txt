cmake_minimum_required(VERSION 3.20)
project(DuckyEngine LANGUAGES CXX C)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# ────────────────────────────────────────────────────────────────
# Szukamy zależności
# ────────────────────────────────────────────────────────────────
find_package(OpenGL REQUIRED)
find_package(glfw3 3.3 REQUIRED)

# ────────────────────────────────────────────────────────────────
# Źródła
# ────────────────────────────────────────────────────────────────
add_executable(DuckyEngine
        main.cpp
        glad/src/glad.c
        src/core/window/Window.hpp
        src/core/window/Window.cpp
)

target_include_directories(DuckyEngine PRIVATE glad/include)

# ────────────────────────────────────────────────────────────────
# Linkowanie – różne na różnych platformach
# ────────────────────────────────────────────────────────────────
target_link_libraries(DuckyEngine PRIVATE
        glfw
        OpenGL::GL
)

if(APPLE)
    target_link_libraries(DuckyEngine PRIVATE
            "-framework OpenGL"
            "-framework Cocoa"
            "-framework IOKit"
            "-framework CoreVideo"
    )
else()
    target_link_libraries(DuckyEngine PRIVATE dl)
endif()

# ────────────────────────────────────────────────────────────────
# Flagi kompilacji / definicje
# ────────────────────────────────────────────────────────────────
if(APPLE)
    target_compile_definitions(DuckyEngine PRIVATE
            GL_MAJOR_VERSION=4
            GL_MINOR_VERSION=1
    )
else()
    target_compile_definitions(DuckyEngine PRIVATE
            GL_MAJOR_VERSION=4
            GL_MINOR_VERSION=1
    )
endif()