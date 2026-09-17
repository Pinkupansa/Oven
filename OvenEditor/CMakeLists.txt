project(OvenEditor LANGUAGES CXX)

file(GLOB_RECURSE OvenEditor_SOURCES
    "src/*.cpp"
)

add_executable(OvenEditor
    ${OvenEditor_SOURCES}
)

target_include_directories(OvenEditor PRIVATE include)

target_link_libraries(OvenEditor
    PRIVATE
        OvenEngine
        glm
        imgui
        imguizmo
)

target_compile_definitions(OvenEditor
    PUBLIC
        $<$<PLATFORM_ID:Windows>:OVEN_PLATFORM_WINDOWS>
    PRIVATE
        $<$<CONFIG:Debug>:OVEN_DEBUG>
        $<$<CONFIG:Release>:OVEN_RELEASE>
        $<$<CONFIG:Dist>:OVEN_DIST>
)

file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/assets 
     DESTINATION ${CMAKE_CURRENT_BINARY_DIR})