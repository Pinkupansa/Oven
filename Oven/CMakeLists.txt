cmake_minimum_required(VERSION 3.15) # Requis pour CMAKE_MSVC_RUNTIME_LIBRARY

# 1. MODIFICATION : Ajout de OBJCXX pour compiler des fichiers .mm sur macOS
project(OvenEngine LANGUAGES C CXX OBJCXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# -----------------------------
# Configuration Globale : Static Runtime
# -----------------------------
set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")

if(APPLE)
elseif(UNIX)
    add_compile_options("-static-libstdc++" "-static-libgcc")
    add_link_options("-static-libstdc++" "-static-libgcc")
endif() 


# -----------------------------
# Frameworks Apple
# -----------------------------
if(APPLE)
    find_library(APPKIT_LIBRARY AppKit REQUIRED)
    find_library(COCOA_LIBRARY Cocoa REQUIRED)
    find_library(UTI_LIBRARY UniformTypeIdentifiers REQUIRED)
endif()
# -----------------------------
# Sources communes (hors Platform)
# -----------------------------
file(GLOB_RECURSE ENGINE_SOURCES "src/*.cpp")
file(GLOB_RECURSE ENGINE_HEADERS "Oven/*.h")

list(FILTER ENGINE_SOURCES EXCLUDE REGEX ".*/Platform/.*")
list(FILTER ENGINE_HEADERS EXCLUDE REGEX ".*/Platform/.*")

# -----------------------------
# Sources spécifiques à la plateforme
# -----------------------------
if(APPLE)
    # 2. MODIFICATION : Inclure aussi les fichiers *.mm (Objective-C++)
    file(GLOB PLATFORM_SOURCES "src/Platform/Mac/*.cpp" "src/Platform/Mac/*.mm")
    file(GLOB PLATFORM_HEADERS "Oven/Platform/Mac/*.h")
elseif(WIN32)
    file(GLOB PLATFORM_SOURCES "src/Platform/Windows/*.cpp")
    file(GLOB PLATFORM_HEADERS "Oven/Platform/Windows/*.h")
elseif(UNIX)
    file(GLOB PLATFORM_SOURCES "src/Platform/Linux/*.cpp")
    file(GLOB PLATFORM_HEADERS "Oven/Platform/Linux/*.h")
else()
    message(FATAL_ERROR "Plateforme non supportée !")
endif()

file(GLOB OPENGL_SOURCES "src/Platform/OpenGL/*.cpp")
file(GLOB OPENGL_HEADERS "Oven/Platform/OpenGL/*.h")
message(STATUS "Mac sources: ${PLATFORM_SOURCES}")
list(APPEND ENGINE_SOURCES ${PLATFORM_SOURCES} ${OPENGL_SOURCES})
list(APPEND ENGINE_HEADERS ${PLATFORM_HEADERS} ${OPENGL_HEADERS})

# -----------------------------
# Dépendances (Vendor)
# -----------------------------
add_subdirectory(vendor/spdlog)
add_subdirectory(vendor/glfw)
add_subdirectory(vendor/imgui)
add_subdirectory(vendor/glm)
add_subdirectory(vendor/yaml-cpp)
add_subdirectory(vendor/imguizmo)
list(APPEND ENGINE_HEADERS "vendor/stb_image/stb_image.h")
list(APPEND ENGINE_SOURCES "vendor/stb_image/stb_image.cpp")
find_package(OpenGL REQUIRED)

add_library(glad STATIC vendor/glad/src/glad.c)
target_include_directories(glad PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/vendor/glad/include
)

# -----------------------------
# Library OvenEngine
# -----------------------------
add_library(OvenEngine STATIC
    ${ENGINE_SOURCES}
    ${ENGINE_HEADERS}
)

target_precompile_headers(OvenEngine PRIVATE Oven/ovenpch.h)

# ImGui Custom Loader : Évite les conflits de macros gl entre GLAD et ImGui
target_compile_definitions(OvenEngine PUBLIC IMGUI_IMPL_OPENGL_LOADER_CUSTOM)
target_compile_definitions(OvenEngine PUBLIC GL_SILENCE_DEPRECATION)

# -----------------------------
# Include dirs
# -----------------------------
target_include_directories(OvenEngine
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
        ${CMAKE_CURRENT_SOURCE_DIR}/vendor/entt
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src
)

# -----------------------------
# Defines
# -----------------------------
target_compile_definitions(OvenEngine
    PUBLIC
        $<$<PLATFORM_ID:Darwin>:OVEN_PLATFORM_MAC>
        $<$<PLATFORM_ID:Windows>:OVEN_PLATFORM_WINDOWS>
        $<$<PLATFORM_ID:Linux>:OVEN_PLATFORM_LINUX>
    PRIVATE
        OVEN_BUILD_DLL
        $<$<CONFIG:Debug>:OVEN_DEBUG>
        $<$<CONFIG:Release>:OVEN_RELEASE>
        $<$<CONFIG:Dist>:OVEN_DIST>
)

# -----------------------------
# Links
# -----------------------------
target_link_libraries(OvenEngine
    PUBLIC
        spdlog
        imgui
    PRIVATE
        glfw
        OpenGL::GL
        glm
        glad
        yaml-cpp
        imguizmo
)

target_link_libraries(imguizmo PUBLIC imgui)
if(APPLE)
    target_link_libraries(OvenEngine PRIVATE
        ${APPKIT_LIBRARY}
        ${COCOA_LIBRARY}
        ${UTI_LIBRARY}
    )
endif()
# -----------------------------
# Flags de compilation
# -----------------------------
if(MSVC)
    target_compile_options(OvenEngine PRIVATE
        /W4
        $<$<CONFIG:Debug>:/Zi /Od>
        $<$<CONFIG:Release>:/O2>
        $<$<CONFIG:Dist>:/O2>
    )
else()
    target_compile_options(OvenEngine PRIVATE
        -Wall -Wextra -Wpedantic -Wno-unused-parameter
        $<$<CONFIG:Debug>:-g -O0>
        $<$<CONFIG:Release>:-O3>
        $<$<CONFIG:Dist>:-O3>
    )
endif()

# -----------------------------
# Visibilité des symboles
# -----------------------------
set_target_properties(OvenEngine PROPERTIES
    CXX_VISIBILITY_PRESET hidden
    VISIBILITY_INLINES_HIDDEN YES
)