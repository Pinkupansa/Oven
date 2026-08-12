#include "Oven/ovenpch.h"
#include "Oven/Renderer/Texture.h"
#include "Oven/Renderer/Renderer.h"
#include "Oven/Platform/OpenGL/OpenGLTexture.h"
namespace Oven
{
Ref<Texture2D> Texture2D::Create(const std::string& path)
{
    switch (Renderer::GetBackend())
    {
        case RendererAPI::RenderingBackend::None:
            OVEN_CORE_ASSERT(false, "RenderingBackend::None is currently not supported !");
            return nullptr;
        case RendererAPI::RenderingBackend::OpenGL:
            return CreateRef<OpenGLTexture2D>(path);
    }
    OVEN_CORE_ASSERT(false, "Unknown RenderingBackend !");
    return nullptr;
}
Ref<Texture2D> Texture2D::Create(uint32_t width, uint32_t height)
{
    switch (Renderer::GetBackend())
    {
        case RendererAPI::RenderingBackend::None:
            OVEN_CORE_ASSERT(false, "RenderingBackend::None is currently not supported !");
            return nullptr;
        case RendererAPI::RenderingBackend::OpenGL:
            return CreateRef<OpenGLTexture2D>(width, height);
    }
    OVEN_CORE_ASSERT(false, "Unknown RenderingBackend !");
    return nullptr;
}
} // namespace Oven