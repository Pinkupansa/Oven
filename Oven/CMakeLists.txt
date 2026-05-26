project(OvenEngine LANGUAGES C CXX)

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

list(APPEND ENGINE_SOURCES ${PLATFORM_SOURCES})
list(APPEND ENGINE_HEADERS ${PLATFORM_HEADERS})

# -----------------------------
# Library
# -----------------------------
add_library(OvenEngine SHARED
    ${ENGINE_SOURCES}
    ${ENGINE_HEADERS}
)

target_precompile_headers(OvenEngine PRIVATE Oven/ovenpch.h)

# -----------------------------
# Dépendances
# -----------------------------
add_subdirectory(vendor/spdlog)
add_subdirectory(vendor/glfw)

find_package(OpenGL REQUIRED)

add_library(glad STATIC vendor/glad/src/glad.c)
target_include_directories(glad PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/vendor/glad/include
)

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
    PRIVATE
        glfw
        glad
        OpenGL::GL
)

# -----------------------------
# Flags de compilation
# -----------------------------
if(MSVC)
    target_compile_options(OvenEngine PRIVATE
        /W4
        $<$<CONFIG:Debug>:/MDd /Zi /Od>
        $<$<CONFIG:Release>:/MD /O2>
        $<$<CONFIG:Dist>:/MD /O2>
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