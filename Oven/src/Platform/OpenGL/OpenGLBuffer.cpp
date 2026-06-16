#include "Oven/ovenpch.h"
#include "Oven/Platform/OpenGL/OpenGLBuffer.h"
#include "Oven/Platform/OpenGL/OpenGLMacros.h"
#include <glad/glad.h>
#include "Oven/Log.h"
namespace Oven{
    ///////////////////////////////////////
    ////////VertexBuffer///////////////////
    ///////////////////////////////////////
    OpenGLVertexBuffer::OpenGLVertexBuffer(float* vertices, uint32_t size) {
        GL_CALL(glGenBuffers(1, &m_RendererID));
        GL_CALL(glBindBuffer(GL_ARRAY_BUFFER, m_RendererID));
        GL_CALL(glBufferData(GL_ARRAY_BUFFER, size, vertices, GL_STATIC_DRAW));
    }

    OpenGLVertexBuffer::~OpenGLVertexBuffer(){
        GL_CALL(glDeleteBuffers(1, &m_RendererID));
    }

    void OpenGLVertexBuffer::Bind() const{
        GL_CALL(glBindBuffer(GL_ARRAY_BUFFER, m_RendererID));
    }
    void OpenGLVertexBuffer::Unbind() const{
        GL_CALL(glBindBuffer(GL_ARRAY_BUFFER, 0));
    }

    ///////////////////////////////////////
    ////////IndexBuffer////////////////////
    ///////////////////////////////////////
    OpenGLIndexBuffer::OpenGLIndexBuffer(uint32_t* indices, uint32_t count) : m_Count(count){
        GL_CALL(glGenBuffers(1, &m_RendererID));
        GL_CALL(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_RendererID));
        GL_CALL(glBufferData(GL_ELEMENT_ARRAY_BUFFER, count*sizeof(uint32_t), indices, GL_STATIC_DRAW));
    }

    OpenGLIndexBuffer::~OpenGLIndexBuffer(){
        GL_CALL(glDeleteBuffers(1, &m_RendererID));
    }

    void OpenGLIndexBuffer::Bind() const{
        GL_CALL(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_RendererID));
    }
    void OpenGLIndexBuffer::Unbind() const{
        GL_CALL(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0));
    }  
}