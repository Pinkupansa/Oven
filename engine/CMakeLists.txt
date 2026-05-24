# --- 1. Nom de la librairie ---
project(OvenEngine LANGUAGES CXX)

# --- 2. Récupérer les sources ---
file(GLOB PRIVATE_SOURCES "src/*.cpp")
file(GLOB PUBLIC_HEADERS "include/engine/*.hpp")

# --- 3. Créer la librairie (STATIC ou SHARED) ---
# Dans engine/CMakeLists.txt
add_library(OvenEngine SHARED ${PRIVATE_SOURCES} ${PUBLIC_HEADERS})

# Définir ENGINE_BUILD_DLL UNIQUEMENT pour la librairie (pas pour le sandbox)
target_compile_definitions(OvenEngine PRIVATE OVEN_BUILD_DLL)
# --- 4. Inclure les headers publics ---
target_include_directories(OvenEngine
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include  # Chemin pour les utilisateurs de la librairie
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src       # Chemin interne (non exposé)
)

# --- 5. Lier les dépendances (GLFW, GLEW, OpenGL, etc.) ---
find_package(glfw3 REQUIRED)
find_package(GLEW REQUIRED)
find_package(OpenGL REQUIRED)

target_link_libraries(OvenEngine
    PRIVATE
        glfw
        GLEW::GLEW
        OpenGL::GL
)

# --- 6. Définir les options de compilation ---
target_compile_options(OvenEngine
    PRIVATE
        -Wall -Wextra -Wpedantic
)

# --- 7. Exporter les symboles (si librairie SHARED) ---
# set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS TRUE)  # Pour Windows (DLL)
# set(CMAKE_VISIBILITY_PRESET hidden)         # Masquer les symboles par défaut
# set(CMAKE_VISIBILITY_INLINES_HIDDEN ON)