#include "Oven/ovenpch.h"
#include "Oven/Renderer/Shader.h"
#include "Oven/Renderer/Renderer.h"
#include "Oven/Log.h"
#include "Oven/Platform/OpenGL/OpenGLShader.h"
namespace Oven{


    Shader* Shader::Create(std::string &vertexSrc, std::string &fragmentSrc){
        switch (Renderer::GetGraphicsAPI()){
            case RendererAPI::RenderingBackend::None: OVEN_CORE_ASSERT(false, "GraphicsAPI::None is currently not supported !"); return nullptr;
            case RendererAPI::RenderingBackend::OpenGL: return new OpenGLShader(vertexSrc, fragmentSrc);
        }
        OVEN_CORE_ASSERT(false, "Unknown GraphicsAPI");
        return nullptr;
    }
}