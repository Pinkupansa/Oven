cmake_minimum_required(VERSION 3.10)
project(OpenGLApp LANGUAGES CXX)

# --- 1. Configurer les chemins ---
set(SOURCES_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src)
set(INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/include)

# --- 2. Trouver GLFW ---
find_package(glfw3 REQUIRED)

# --- 3. Trouver OpenGL (pour macOS, on utilise les frameworks système) ---
find_package(OpenGL REQUIRED)  # <-- Ajoute cette ligne

# --- 4. Récupérer les sources ---
file(GLOB SOURCES "${SOURCES_DIR}/*.cpp")

# --- 5. Créer l'exécutable ---
add_executable(${PROJECT_NAME} ${SOURCES})

# --- 6. Ajouter include/ ---
target_include_directories(${PROJECT_NAME} PRIVATE ${INCLUDE_DIR})

# --- 7. Lier les bibliothèques ---
target_link_libraries(${PROJECT_NAME}
    PRIVATE
        glfw
        OpenGL::GL  # <-- Maintenant ça marchera
)

# --- 8. Gestion spécifique macOS ---
if(APPLE)
    find_library(COCOA_LIBRARY Cocoa)
    find_library(OPENGL_LIBRARY OpenGL)
    find_library(IOKIT_LIBRARY IOKit)
    target_link_libraries(${PROJECT_NAME}
        PRIVATE
            ${COCOA_LIBRARY}
            ${OPENGL_LIBRARY}
            ${IOKIT_LIBRARY}
    )
endif()

# --- 9. Options de compilation ---
target_compile_options(${PROJECT_NAME} PRIVATE -Wall -Wextra -Wpedantic)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)