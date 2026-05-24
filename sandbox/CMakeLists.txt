project(Sandbox LANGUAGES CXX)

# --- 1. Récupérer les sources du sandbox ---
file(GLOB SOURCES "src/*.cpp")

# --- 2. Créer l'exécutable ---
add_executable(Sandbox ${SOURCES})

# --- 3. Lier la librairie Engine et ses dépendances ---
target_link_libraries(Sandbox
    PRIVATE
        OvenEngine  
)

# --- 4. Inclure les headers de Engine ---
target_include_directories(Sandbox
    PRIVATE
        ${CMAKE_SOURCE_DIR}/engine
)