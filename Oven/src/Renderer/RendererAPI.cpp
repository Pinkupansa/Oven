#include "Oven/ovenpch.h"
#include "Oven/Renderer/RendererAPI.h" 

namespace Oven{
    RendererAPI::GraphicsAPI RendererAPI::s_GraphicsAPI = RendererAPI::GraphicsAPI::OpenGL;
}