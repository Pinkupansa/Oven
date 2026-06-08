cmake_minimum_required(VERSION 3.15) # Requis pour CMAKE_MSVC_RUNTIME_LIBRARY
project(OvenEngine LANGUAGES C CXX)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
# -----------------------------
# Configuration Globale : Static Runtime
# -----------------------------
# Force le runtime statique global (ex: /MT et /MTd sur Windows/MSVC)
set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")

if(APPLE)
#Sur Apple, impossible de faire du 100% static 
elseif(UNIX)
add_compile_options("-static-libstdc++" "-static-libgcc")
add_link_options("-static-libstdc++" "-static-libgcc")
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
    file(GLOB PLATFORM_SOURCES "src/Platform/Mac/*.cpp")
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

# -----------------------------
# Include dirs
# -----------------------------
target_include_directories(OvenEngine
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
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
        glad
        OpenGL::GL
        glm
)

# -----------------------------
# Flags de compilation
# -----------------------------
if(MSVC)
    target_compile_options(OvenEngine PRIVATE
        /W4
        $<$<CONFIG:Debug>:/Zi /Od>   # NETTOYÉ : Retrait de /MDd pour ne pas écraser le runtime statique
        $<$<CONFIG:Release>:/O2>     # NETTOYÉ : Retrait de /MD
        $<$<CONFIG:Dist>:/O2>        # NETTOYÉ : Retrait de /MD
    )
else()
    target_compile_options(OvenEngine PRIVATE
        -Wall -Wextra -Wpedantic
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