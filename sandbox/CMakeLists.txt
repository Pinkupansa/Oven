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
)

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