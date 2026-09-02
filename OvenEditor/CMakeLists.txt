project(OvenEditor LANGUAGES CXX)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
# -----------------------------
# Sources
# -----------------------------
file(GLOB_RECURSE OvenEditor_SOURCES
    "src/*.cpp"
)

# -----------------------------
# Executable
# -----------------------------
add_executable(OvenEditor
    ${OvenEditor_SOURCES}
)

target_include_directories(OvenEditor PRIVATE include)
# -----------------------------
# Liaison avec le moteur
# -----------------------------
# Grâce à 'PUBLIC' dans le projet Oven, OvenEditor reçoit automatiquement 
# l'accès aux includes d'Oven et à spdlog !
target_link_libraries(OvenEditor
    PRIVATE
        OvenEngine
        glm
        imgui
        imguizmo
)

#Je sais pas si c'est ok de link glm et imgui alors que c'est compilé dans oven
#en terme d'architecture

# -----------------------------
# Définitions de préprocesseur pour la OvenEditor
# -----------------------------
target_compile_definitions(OvenEditor
    PUBLIC
        $<$<PLATFORM_ID:Windows>:OVEN_PLATFORM_WINDOWS>
    PRIVATE
        $<$<CONFIG:Debug>:OVEN_DEBUG>
        $<$<CONFIG:Release>:OVEN_RELEASE>
        $<$<CONFIG:Dist>:OVEN_DIST>
)

# Copy the assets folder to the build directory where the executable lives
file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/assets 
     DESTINATION ${CMAKE_CURRENT_BINARY_DIR})