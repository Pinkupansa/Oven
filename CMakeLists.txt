cmake_minimum_required(VERSION 3.10)
project(Oven LANGUAGES CXX)

# --- 1. Activer C++17 pour tout le projet ---
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# --- 2. Ajouter les sous-projets ---
add_subdirectory(engine)    # La librairie
add_subdirectory(sandbox)  # L'exécutable