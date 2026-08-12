#include "Oven/ovenpch.h"
#include "Oven/Renderer/Buffer.h"
#include "Oven/Renderer/Renderer.h"
#include "Oven/Core/Log.h"
#include "Oven/Platform/OpenGL/OpenGLBuffer.h"
namespace Oven
{

Ref<VertexBuffer> VertexBuffer::Create(float* vertices, uint32_t size)
{

    switch (Renderer::GetBackend())
    {
        case RendererAPI::RenderingBackend::None:
            OVEN_CORE_ASSERT(false, "RenderingBackend::None is currently not supported! ");
            return nullptr;
        case RendererAPI::RenderingBackend::OpenGL:
            return CreateRef<OpenGLVertexBuffer>(vertices, size);
    }
    OVEN_CORE_ASSERT(false, "Unknown RenderingBackend!");
    return nullptr;
}

Ref<VertexBuffer> VertexBuffer::Create(uint32_t size)
{

    switch (Renderer::GetBackend())
    {
        case RendererAPI::RenderingBackend::None:
            OVEN_CORE_ASSERT(false, "RenderingBackend::None is currently not supported! ");
            return nullptr;
        case RendererAPI::RenderingBackend::OpenGL:
            return CreateRef<OpenGLVertexBuffer>(size);
    }
    OVEN_CORE_ASSERT(false, "Unknown RenderingBackend!");
    return nullptr;
}

Ref<IndexBuffer> IndexBuffer::Create(uint32_t* indices, uint32_t count)
{

    switch (Renderer::GetBackend())
    {
        case RendererAPI::RenderingBackend::None:
            OVEN_CORE_ASSERT(false, "RenderingBackend::None is currently not supported! ");
            return nullptr;
        case RendererAPI::RenderingBackend::OpenGL:
            return CreateRef<OpenGLIndexBuffer>(indices, count);
    }
    OVEN_CORE_ASSERT(false, "Unknown RenderingBackend !");
    return nullptr;
}
} // namespace Oven