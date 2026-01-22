cmake_minimum_required(VERSION 3.16)
project(DuckyEngine LANGUAGES CXX)

# --- C++ ---
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# --- CrossPlatform settings ---
if(MSVC)
    add_compile_options(/W4 /permissive-)
else()
    add_compile_options(-Wall -Wextra -pedantic)
endif()

## --- GLAD ---
## GLAD has to be in 'external/glad'!!!
#add_library(glad STATIC
#        external/glad/src/glad.c
#)
#target_include_directories(glad PUBLIC
#        external/glad/include
#)
#
## --- GLM ---
## GLM has to be in 'external/glm'!!!!
#add_library(glm INTERFACE)
#target_include_directories(glm INTERFACE
#        external/glm
#)
#
## --- OpenGL ---
#find_package(OpenGL REQUIRED)
#
## --- Executables ---
add_executable(DuckyEngine
        main.cpp
)

## --- Links ---
#target_link_libraries(DuckyEngine
#        PRIVATE
#        glad
#        glm
#        OpenGL::GL
#)
#
# --- MacOS Settings---
if(APPLE)
    find_library(COCOA_LIBRARY Cocoa)
    find_library(IOKIT_LIBRARY IOKit)
    find_library(COREVIDEO_LIBRARY CoreVideo)
    find_library(FOUNDATION_LIBRARY Foundation)

    target_link_libraries(DuckyEngine PRIVATE
            ${COCOA_LIBRARY}
            ${IOKIT_LIBRARY}
            ${COREVIDEO_LIBRARY}
            ${FOUNDATION_LIBRARY}
    )
endif()

# --- Windows Settings ---
if(WIN32)
    target_link_libraries(MyEngine PRIVATE opengl32)
endif()
