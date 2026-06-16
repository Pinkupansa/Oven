#include "Oven/ovenpch.h"
#include "Oven/Renderer/RenderCommand.h"
#include "Oven/Platform/OpenGL/OpenGLRendererAPI.h"

namespace Oven{
    RendererAPI* RenderCommand::s_RendererAPI = new OpenGLRendererAPI();
}