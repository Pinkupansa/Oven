#pragma once

#include <cstdint>

namespace Oven
{

class OpenGLCaps
{
public:
    // À appeler une seule fois, après la création du contexte et gladLoadGL.
    static void Init();

    static bool SupportsSpirv() { return s_SupportsSpirv; }
    static uint32_t GLSLVersion() { return s_GLSLVersion; } // 410, 450, 460...

private:
    inline static bool s_SupportsSpirv = false;
    inline static uint32_t s_GLSLVersion = 330;
};

} // namespace Oven