#include "Oven/ovenpch.h"
#include "Oven/Renderer/VertexArray.h"
#include "Oven/Renderer/Renderer.h"
#include "Oven/Log.h"
#include "Oven/Platform/OpenGL/OpenGLVertexArray.h"
namespace Oven{


    VertexArray* VertexArray::Create(){
        switch (Renderer::GetGraphicsAPI()){
            case RendererAPI::RenderingBackend::None: OVEN_CORE_ASSERT(false, "RenderingBackend::None is currently not supported !"); return nullptr; 
            case RendererAPI::RenderingBackend::OpenGL: return new OpenGLVertexArray(); 
        }
        OVEN_CORE_ASSERT(false, "Unknown RenderingBackend !"); 
        return nullptr;
    }
    
}