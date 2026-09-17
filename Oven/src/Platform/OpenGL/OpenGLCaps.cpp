// OpenGLCaps.cpp
#include "Oven/ovenpch.h"
#include "Oven/Platform/OpenGL/OpenGLCaps.h"
#include "Oven/Core/Log.h"
#include <glad/glad.h>

namespace Oven
{

void OpenGLCaps::Init()
{
    GLint major = 0, minor = 0;
    glGetIntegerv(GL_MAJOR_VERSION, &major);
    glGetIntegerv(GL_MINOR_VERSION, &minor);

    s_GLSLVersion = (uint32_t)(major * 100 + minor * 10);

    // On teste l'extension ET le pointeur de fonction : certains drivers
    // annoncent ARB_gl_spirv sans réellement l'implémenter.
    s_SupportsSpirv = (glShaderBinary != nullptr) && (glSpecializeShader != nullptr);
    OVEN_CORE_INFO(
        "OpenGL {0}.{1} | GLSL {2} | SPIR-V: {3}",
        major,
        minor,
        s_GLSLVersion,
        s_SupportsSpirv ? "oui" : "non (fallback GLSL texte)"
    );
}

} // namespace Oven