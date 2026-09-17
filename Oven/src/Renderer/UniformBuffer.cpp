#include "Oven/ovenpch.h"
#include "Oven/Renderer/UniformBuffer.h"

#include "Oven/Renderer/Renderer.h"
#include "Oven/Platform/OpenGL/OpenGLUniformBuffer.h"

namespace Oven
{

Ref<UniformBuffer> UniformBuffer::Create(uint32_t size, uint32_t binding)
{
    switch (Renderer::GetBackend())
    {
        case RendererAPI::RenderingBackend::None:
            OVEN_CORE_ASSERT(false, "RendererAPI::None is currently not supported!");
            return nullptr;
        case RendererAPI::RenderingBackend::OpenGL:
            return CreateRef<OpenGLUniformBuffer>(size, binding);
    }

    OVEN_CORE_ASSERT(false, "Unknown RendererAPI!");
    return nullptr;
}

} // namespace Oven