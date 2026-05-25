project(OvenEngine LANGUAGES CXX)

# -----------------------------
# Sources
# -----------------------------
file(GLOB_RECURSE ENGINE_SOURCES
    "src/*.cpp"
)

file(GLOB_RECURSE ENGINE_HEADERS
    "include/*.hpp"
    "src/*.h"      # Inclus au cas où tu as des headers privés dans src/
)

# -----------------------------
# Library (DLL)
# -----------------------------
add_library(OvenEngine SHARED
    ${ENGINE_SOURCES}
    ${ENGINE_HEADERS}
)

# -----------------------------
# Dépendances internes & externes
# -----------------------------
# spdlog (interne au projet)
add_subdirectory(vendor/spdlog)

# Dépendances système (Assure-toi de les avoir installées via vcpkg ou ton système)
find_package(glfw3 REQUIRED)
find_package(GLEW REQUIRED)
find_package(OpenGL REQUIRED)

# -----------------------------
# Chemins d'inclusion (Include Dirs)
# -----------------------------
target_include_directories(OvenEngine
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src
)

# -----------------------------
# Définitions de préprocesseur (Le Coeur de ta question)
# -----------------------------
target_compile_definitions(OvenEngine
    PUBLIC
        # Détection automatique de Windows (Plus propre que de le hardcoder)
        $<$<PLATFORM_ID:Windows>:OVEN_PLATFORM_WINDOWS>
    
    PRIVATE
        # Spécifique à la création de la DLL d'Oven
        OVEN_BUILD_DLL

        # Configurations de Build (Gérées dynamiquement par CMake)
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
        GLEW::GLEW
        OpenGL::GL
)

# -----------------------------
# Options du compilateur (Drapeaux /Flags)
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
        -Wall
        -Wextra
        -Wpedantic
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