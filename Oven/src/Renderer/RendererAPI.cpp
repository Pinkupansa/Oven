#include "Oven/ovenpch.h"
#include "Oven/Renderer/RendererAPI.h" 

namespace Oven{
    RendererAPI::RenderingBackend RendererAPI::s_RenderingBackend = RendererAPI::RenderingBackend::OpenGL;
}