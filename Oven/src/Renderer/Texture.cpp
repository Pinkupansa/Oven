#include "Oven/ovenpch.h"
#include "Oven/Renderer/Texture.h"
#include "Oven/Renderer/Renderer.h"
#include "Oven/Platform/OpenGL/OpenGLTexture.h"
namespace Oven{
    Ref<Texture2D> Texture2D::Create(const std::string& path){
        switch (Renderer::GetBackend()){
            case RendererAPI::RenderingBackend::None: OVEN_CORE_ASSERT(false, "RenderingBackend::None is currently not supported !"); return nullptr; 
            case RendererAPI::RenderingBackend::OpenGL: return std::make_shared<OpenGLTexture2D>(path); 
        }
        OVEN_CORE_ASSERT(false, "Unknown RenderingBackend !"); 
        return nullptr;
    }
}