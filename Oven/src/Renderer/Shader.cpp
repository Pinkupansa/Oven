#include "Oven/Renderer/Shader.h"
#include "Oven/Renderer/Renderer.h"
#include "Oven/Log.h"
#include "Oven/Platform/OpenGL/OpenGLShader.h"
namespace Oven{


    Shader* Shader::Create(std::string &vertexSrc, std::string &fragmentSrc){
        switch (Renderer::GetRenderingAPI()){
            case RenderingAPI::None: OVEN_CORE_ASSERT(false, "RenderingAPI::None is currently not supported !"); return nullptr;
            case RenderingAPI::OpenGL: return new OpenGLShader(vertexSrc, fragmentSrc);
        }
        return nullptr;
    }

}