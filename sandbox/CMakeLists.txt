project(Sandbox LANGUAGES CXX)

# -----------------------------
# Sources
# -----------------------------
file(GLOB_RECURSE SANDBOX_SOURCES
    "src/*.cpp"
    "include/*.hpp"
)

# -----------------------------
# Executable
# -----------------------------
add_executable(Sandbox
    ${SANDBOX_SOURCES}
)

# -----------------------------
# Liaison avec le moteur
# -----------------------------
# Grâce à 'PUBLIC' dans le projet Oven, Sandbox reçoit automatiquement 
# l'accès aux includes d'Oven et à spdlog !
target_link_libraries(Sandbox
    PRIVATE
        OvenEngine
        glm
        imgui
)

#Je sais pas si c'est ok de link glm et imgui alors que c'est compilé dans oven
#en terme d'architecture

# -----------------------------
# Définitions de préprocesseur pour la Sandbox
# -----------------------------
target_compile_definitions(Sandbox
    PUBLIC
        $<$<PLATFORM_ID:Windows>:OVEN_PLATFORM_WINDOWS>
    PRIVATE
        $<$<CONFIG:Debug>:OVEN_DEBUG>
        $<$<CONFIG:Release>:OVEN_RELEASE>
        $<$<CONFIG:Dist>:OVEN_DIST>
)