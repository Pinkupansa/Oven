cmake_minimum_required(VERSION 3.15)
project(OvenEngine LANGUAGES C CXX OBJCXX)

# -----------------------------
# Configuration Globale : Static Runtime
# -----------------------------
set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")

if(UNIX AND NOT APPLE)
    add_compile_options("-static-libstdc++" "-static-libgcc")
    add_link_options("-static-libstdc++" "-static-libgcc")
endif() 

# -----------------------------
# Apple Frameworks
# -----------------------------
if(APPLE)
    find_library(APPKIT_LIBRARY AppKit REQUIRED)
    find_library(COCOA_LIBRARY Cocoa REQUIRED)
    find_library(UTI_LIBRARY UniformTypeIdentifiers REQUIRED)
endif()

# -----------------------------
# Vulkan & Shaderc Detection
# -----------------------------
if(APPLE AND NOT DEFINED ENV{VULKAN_SDK})
    file(GLOB VULKAN_SDK_VERSIONS "$ENV{HOME}/VulkanSDK/*/*/macOS")
    if(VULKAN_SDK_VERSIONS)
        list(GET VULKAN_SDK_VERSIONS -1 LATEST_VULKAN_SDK)
        set(ENV{VULKAN_SDK} "${LATEST_VULKAN_SDK}")
        set(Vulkan_ROOT "${LATEST_VULKAN_SDK}")
        list(APPEND CMAKE_PREFIX_PATH "${LATEST_VULKAN_SDK}")
    endif()
elseif(DEFINED ENV{VULKAN_SDK})
    set(Vulkan_ROOT "$ENV{VULKAN_SDK}")
    list(APPEND CMAKE_PREFIX_PATH "$ENV{VULKAN_SDK}")
endif()

find_package(Vulkan QUIET COMPONENTS Shaderc)

if(NOT Vulkan_Shaderc_FOUND)
    message(STATUS "Shaderc non détecté par le module standard, bascule sur la recherche manuelle...")
    
    find_package(Vulkan REQUIRED)

    find_path(Vulkan_Shaderc_INCLUDE_DIR
        NAMES shaderc/shaderc.hpp
        HINTS $ENV{VULKAN_SDK}/include /opt/homebrew/include /usr/local/include
    )

    find_library(Vulkan_Shaderc_LIBRARY
        NAMES shaderc_shared shaderc_combined shaderc
        HINTS $ENV{VULKAN_SDK}/lib /opt/homebrew/lib /usr/local/lib
    )

    if(Vulkan_Shaderc_INCLUDE_DIR AND Vulkan_Shaderc_LIBRARY)
        set(Vulkan_Shaderc_FOUND TRUE)
        if(NOT TARGET Vulkan::Shaderc)
            add_library(Vulkan::Shaderc UNKNOWN IMPORTED)
            set_target_properties(Vulkan::Shaderc PROPERTIES
                IMPORTED_LOCATION "${Vulkan_Shaderc_LIBRARY}"
                INTERFACE_INCLUDE_DIRECTORIES "${Vulkan_Shaderc_INCLUDE_DIR}"
            )
        endif()
    else()
        message(FATAL_ERROR "Impossible de trouver les en-têtes ou la bibliothèque Shaderc sur le système !")
    endif()
endif()

find_package(OpenGL REQUIRED)

# -----------------------------
# Sources
# -----------------------------
file(GLOB_RECURSE ENGINE_SOURCES "src/*.cpp")
file(GLOB_RECURSE ENGINE_HEADERS "Oven/*.h")

list(FILTER ENGINE_SOURCES EXCLUDE REGEX ".*/Platform/.*")
list(FILTER ENGINE_HEADERS EXCLUDE REGEX ".*/Platform/.*")

if(APPLE)
    file(GLOB PLATFORM_SOURCES "src/Platform/Mac/*.cpp" "src/Platform/Mac/*.mm")
    file(GLOB PLATFORM_HEADERS "Oven/Platform/Mac/*.h")
elseif(WIN32)
    file(GLOB PLATFORM_SOURCES "src/Platform/Windows/*.cpp")
    file(GLOB PLATFORM_HEADERS "Oven/Platform/Windows/*.h")
elseif(UNIX)
    file(GLOB PLATFORM_SOURCES "src/Platform/Linux/*.cpp")
    file(GLOB PLATFORM_HEADERS "Oven/Platform/Linux/*.h")
endif()

file(GLOB OPENGL_SOURCES "src/Platform/OpenGL/*.cpp")
file(GLOB OPENGL_HEADERS "Oven/Platform/OpenGL/*.h")

list(APPEND ENGINE_SOURCES ${PLATFORM_SOURCES} ${OPENGL_SOURCES})
list(APPEND ENGINE_HEADERS ${PLATFORM_HEADERS} ${OPENGL_HEADERS})

# -----------------------------
# Vendor Subdirectories
# -----------------------------
add_subdirectory(vendor/spdlog)
add_subdirectory(vendor/glfw)
add_subdirectory(vendor/imgui)
add_subdirectory(vendor/glm)
add_subdirectory(vendor/yaml-cpp)
add_subdirectory(vendor/imguizmo)

# FIX: Force SPIRV-Cross to use standard std::vector to avoid SmallVector layout crashes
set(SPIRV_CROSS_CLI OFF CACHE BOOL "" FORCE)
set(SPIRV_CROSS_ENABLE_TESTS OFF CACHE BOOL "" FORCE)
add_subdirectory(vendor/SPIRV-Cross)

list(APPEND ENGINE_HEADERS "vendor/stb_image/stb_image.h")
list(APPEND ENGINE_SOURCES "vendor/stb_image/stb_image.cpp")

add_library(glad STATIC vendor/glad/src/glad.c)
target_include_directories(glad PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/vendor/glad/include
)

# -----------------------------
# Target: OvenEngine
# -----------------------------
add_library(OvenEngine STATIC
    ${ENGINE_SOURCES}
    ${ENGINE_HEADERS}
)

target_precompile_headers(OvenEngine PRIVATE Oven/ovenpch.h)

target_include_directories(OvenEngine
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
        ${CMAKE_CURRENT_SOURCE_DIR}/vendor/entt
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src
)

target_compile_definitions(OvenEngine
    PUBLIC
        IMGUI_IMPL_OPENGL_LOADER_CUSTOM
        GL_SILENCE_DEPRECATION
        SPIRV_CROSS_EXCEPTIONS_TO_UNWIND
        $<$<PLATFORM_ID:Darwin>:OVEN_PLATFORM_MAC>
        $<$<PLATFORM_ID:Windows>:OVEN_PLATFORM_WINDOWS>
        $<$<PLATFORM_ID:Linux>:OVEN_PLATFORM_LINUX>
    PRIVATE
        OVEN_BUILD_DLL
        $<$<CONFIG:Debug>:OVEN_DEBUG>
        $<$<CONFIG:Release>:OVEN_RELEASE>
        $<$<CONFIG:Dist>:OVEN_DIST>
)

target_link_libraries(OvenEngine
    PUBLIC
        spdlog
        imgui
        Vulkan::Vulkan
        Vulkan::Shaderc
    PRIVATE
        glfw
        OpenGL::GL
        glm
        glad
        yaml-cpp
        imguizmo
        spirv-cross-core
        spirv-cross-glsl
)

target_link_libraries(imguizmo PUBLIC imgui)

if(APPLE)
    target_link_libraries(OvenEngine PRIVATE
        ${APPKIT_LIBRARY}
        ${COCOA_LIBRARY}
        ${UTI_LIBRARY}
    )
endif()

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

set_target_properties(OvenEngine PROPERTIES
    CXX_VISIBILITY_PRESET hidden
    VISIBILITY_INLINES_HIDDEN YES
)