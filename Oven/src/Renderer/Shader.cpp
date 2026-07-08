#include "Oven/ovenpch.h"
#include "Oven/Renderer/Shader.h"
#include "Oven/Renderer/Renderer.h"
#include "Oven/Log.h"
#include "Oven/Platform/OpenGL/OpenGLShader.h"
namespace Oven{


    Ref<Shader> Shader::Create(const std::string &filepath){
        switch (Renderer::GetBackend()){
            case RendererAPI::RenderingBackend::None: OVEN_CORE_ASSERT(false, "RenderingBackend::None is currently not supported !"); return nullptr;
            case RendererAPI::RenderingBackend::OpenGL: return std::make_shared<OpenGLShader>(filepath);
        }
        OVEN_CORE_ASSERT(false, "Unknown RenderingBackend");
        return nullptr;
    }
    Ref<Shader> Shader::Create(const std::string& name, const std::string &vertexSrc, const std::string &fragmentSrc){
        switch (Renderer::GetBackend()){
            case RendererAPI::RenderingBackend::None: OVEN_CORE_ASSERT(false, "RenderingBackend::None is currently not supported !"); return nullptr;
            case RendererAPI::RenderingBackend::OpenGL: return std::make_shared<OpenGLShader>(name, vertexSrc, fragmentSrc);
        }
        OVEN_CORE_ASSERT(false, "Unknown RenderingBackend");
        return nullptr;
    }

    void ShaderLibrary::Add(const Ref<Shader>& shader){
        auto &name = shader->GetDefaultName();
        Add(name, shader);
    }

    void ShaderLibrary::Add(const std::string& name, const Ref<Shader>& shader){
        OVEN_CORE_ASSERT(!Exists(name), "Shader already exists !");
        m_Shaders[name] = shader;
    }
    Ref<Shader> ShaderLibrary::Load(const std::string& filepath){
        auto shader = Shader::Create(filepath);
        Add(shader);
        return shader;
    }
    Ref<Shader> ShaderLibrary::Load(const std::string& name, const std::string& filepath){
        auto shader = Shader::Create(filepath);
        Add(name, shader);
        return shader;
    }

    Ref<Shader> ShaderLibrary::Get(const std::string& name){ 
        OVEN_CORE_ASSERT(Exists(name), "Shader not found !");
        return m_Shaders[name];
    }

    bool ShaderLibrary::Exists(const std::string& name) const { 
        return m_Shaders.find(name) != m_Shaders.end();
    }
}