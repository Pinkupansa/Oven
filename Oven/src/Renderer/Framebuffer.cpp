#include "Oven/ovenpch.h"
#include "Oven/Renderer/Framebuffer.h"
#include "Oven/Platform/OpenGL/OpenGLFramebuffer.h"
#include "Oven/Renderer/Renderer.h"
namespace Oven
{
Ref<Framebuffer> Framebuffer::Create(const FramebufferSpecs& specs)
{

    switch (Renderer::GetBackend())
    {
        case RendererAPI::RenderingBackend::None:
            OVEN_CORE_ASSERT(false, "RenderingBackend::None is currently not supported! ");
            return nullptr;
        case RendererAPI::RenderingBackend::OpenGL:
            return CreateRef<OpenGLFramebuffer>(specs);
    }
    OVEN_CORE_ASSERT(false, "Unknown RenderingBackend !");
    return nullptr;
}
} // namespace Oven