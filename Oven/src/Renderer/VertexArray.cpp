#include "Oven/Renderer/VertexArray.h"
#include "Oven/Renderer/Renderer.h"
#include "Oven/Log.h"
#include "Oven/Platform/OpenGL/OpenGLVertexArray.h"
namespace Oven{


    VertexArray* VertexArray::Create(){
        switch (Renderer::GetRenderingAPI()){
            case RendererAPI::GraphicsAPI::None: OVEN_CORE_ASSERT(false, "GraphicsAPI::None is currently not supported !"); return nullptr; 
            case RendererAPI::GraphicsAPI::OpenGL: return new OpenGLVertexArray(); 
        }
        OVEN_CORE_ASSERT(false, "Unknown GraphicsAPI !"); 
        return nullptr;
    }
    
}