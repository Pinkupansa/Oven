#include "Oven/ovenpch.h"
#include "Oven/Platform/OpenGL/OpenGLBuffer.h"
#include <glad/glad.h>
#include "Oven/Log.h"
namespace Oven{
    ///////////////////////////////////////
    ////////VertexBuffer///////////////////
    ///////////////////////////////////////
    OpenGLVertexBuffer::OpenGLVertexBuffer(float* vertices, uint32_t size) {
        OVEN_CORE_TRACE("rendererID {0}", (void*)&m_RendererID);
        glBindBuffer(GL_ARRAY_BUFFER, m_RendererID);
        glBufferData(GL_ARRAY_BUFFER, size, vertices, GL_STATIC_DRAW);
    }

    OpenGLVertexBuffer::~OpenGLVertexBuffer(){
        glDeleteBuffers(1, &m_RendererID);
    }

    void OpenGLVertexBuffer::Bind() const{
        glBindBuffer(GL_ARRAY_BUFFER, m_RendererID);
    }
    void OpenGLVertexBuffer::Unbind() const{
        glBindBuffer(GL_ARRAY_BUFFER, 0);
    }

    ///////////////////////////////////////
    ////////IndexBuffer////////////////////
    ///////////////////////////////////////
    OpenGLIndexBuffer::OpenGLIndexBuffer(uint32_t* indices, uint32_t count) : m_Count(count){
        glGenBuffers(1, &m_RendererID);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_RendererID);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, count*sizeof(uint32_t), indices, GL_STATIC_DRAW);
    }

    OpenGLIndexBuffer::~OpenGLIndexBuffer(){
        glDeleteBuffers(1, &m_RendererID);
    }

    void OpenGLIndexBuffer::Bind() const{
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_RendererID);
    }
    void OpenGLIndexBuffer::Unbind() const{
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    }  
}