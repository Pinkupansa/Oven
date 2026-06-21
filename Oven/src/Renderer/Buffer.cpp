#include "Oven/ovenpch.h"
#include "Oven/Renderer/Buffer.h"
#include "Oven/Renderer/Renderer.h"
#include "Oven/Log.h"
#include "Oven/Platform/OpenGL/OpenGLBuffer.h"
namespace Oven{
    VertexBuffer* VertexBuffer::Create(float* vertices, uint32_t size){ 

        switch(Renderer::GetBackend()){
            case RendererAPI::RenderingBackend::None : OVEN_CORE_ASSERT(false, "RenderingBackend::None is currently not supported! "); return nullptr;
            case RendererAPI::RenderingBackend::OpenGL: return new OpenGLVertexBuffer(vertices, size);
        }
        OVEN_CORE_ASSERT(false, "Unknown RenderingBackend!");
        return nullptr;
    }

    IndexBuffer* IndexBuffer::Create(uint32_t* indices, uint32_t size){ 
        
        switch(Renderer::GetBackend()){
            case RendererAPI::RenderingBackend::None : OVEN_CORE_ASSERT(false, "RenderingBackend::None is currently not supported! "); return nullptr;
            case RendererAPI::RenderingBackend::OpenGL: return new OpenGLIndexBuffer(indices, size);
        }
        OVEN_CORE_ASSERT(false, "Unknown RenderingBackend !");
        return nullptr;
    }
}