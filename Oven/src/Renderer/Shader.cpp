#include "Oven/ovenpch.h"
#include "Oven/Renderer/Shader.h"
#include "Oven/Renderer/Renderer.h"
#include "Oven/Log.h"
#include "Oven/Platform/OpenGL/OpenGLShader.h"
namespace Oven{


    Shader* Shader::Create(const std::string &filepath){
        switch (Renderer::GetBackend()){
            case RendererAPI::RenderingBackend::None: OVEN_CORE_ASSERT(false, "RenderingBackend::None is currently not supported !"); return nullptr;
            case RendererAPI::RenderingBackend::OpenGL: return new OpenGLShader(filepath);
        }
        OVEN_CORE_ASSERT(false, "Unknown RenderingBackend");
        return nullptr;
    }
    Shader* Shader::Create(const std::string &vertexSrc, const std::string &fragmentSrc){
        switch (Renderer::GetBackend()){
            case RendererAPI::RenderingBackend::None: OVEN_CORE_ASSERT(false, "RenderingBackend::None is currently not supported !"); return nullptr;
            case RendererAPI::RenderingBackend::OpenGL: return new OpenGLShader(vertexSrc, fragmentSrc);
        }
        OVEN_CORE_ASSERT(false, "Unknown RenderingBackend");
        return nullptr;
    }
}