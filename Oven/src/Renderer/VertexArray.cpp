#include "Oven/ovenpch.h"
#include "Oven/Renderer/VertexArray.h"
#include "Oven/Renderer/Renderer.h"
#include "Oven/Core/Log.h"
#include "Oven/Platform/OpenGL/OpenGLVertexArray.h"
namespace Oven
{

Ref<VertexArray> VertexArray::Create()
{
    switch (Renderer::GetBackend())
    {
        case RendererAPI::RenderingBackend::None:
            OVEN_CORE_ASSERT(false, "RenderingBackend::None is currently not supported !");
            return nullptr;
        case RendererAPI::RenderingBackend::OpenGL:
            return CreateRef<OpenGLVertexArray>();
    }
    OVEN_CORE_ASSERT(false, "Unknown RenderingBackend !");
    return nullptr;
}

} // namespace Oven