#include "Oven/ovenpch.h"
#include "Oven/Platform/OpenGL/OpenGLBuffer.h"
#include "Oven/Platform/OpenGL/OpenGLMacros.h"
#include <glad/glad.h>
#include "Oven/Core/Log.h"
namespace Oven
{
///////////////////////////////////////
////////VertexBuffer///////////////////
///////////////////////////////////////
OpenGLVertexBuffer::OpenGLVertexBuffer(float* vertices, uint32_t size)
{
    OVEN_PROFILE_FUNCTION();
    GL_CALL(glGenBuffers(1, &m_RendererID));
    GL_CALL(glBindBuffer(GL_ARRAY_BUFFER, m_RendererID));
    GL_CALL(glBufferData(GL_ARRAY_BUFFER, size, vertices, GL_STATIC_DRAW));
}

OpenGLVertexBuffer::OpenGLVertexBuffer(uint32_t size)
{
    OVEN_PROFILE_FUNCTION();
    GL_CALL(glGenBuffers(1, &m_RendererID));
    GL_CALL(glBindBuffer(GL_ARRAY_BUFFER, m_RendererID));
    GL_CALL(glBufferData(GL_ARRAY_BUFFER, size, nullptr, GL_DYNAMIC_DRAW));
}

OpenGLVertexBuffer::~OpenGLVertexBuffer()
{
    OVEN_PROFILE_FUNCTION();
    GL_CALL(glDeleteBuffers(1, &m_RendererID));
}

void OpenGLVertexBuffer::Bind() const
{
    OVEN_PROFILE_FUNCTION();
    GL_CALL(glBindBuffer(GL_ARRAY_BUFFER, m_RendererID));
}
void OpenGLVertexBuffer::Unbind() const
{
    OVEN_PROFILE_FUNCTION();
    GL_CALL(glBindBuffer(GL_ARRAY_BUFFER, 0));
}

void OpenGLVertexBuffer::SetData(const void* data, uint32_t size)
{
    OVEN_PROFILE_FUNCTION();
    glBindBuffer(GL_ARRAY_BUFFER, m_RendererID);
    GL_CALL(glBufferSubData(GL_ARRAY_BUFFER, 0, size, data));
}

///////////////////////////////////////
////////IndexBuffer////////////////////
///////////////////////////////////////
OpenGLIndexBuffer::OpenGLIndexBuffer(uint32_t* indices, uint32_t count) : m_Count(count)
{
    OVEN_PROFILE_FUNCTION();
    GL_CALL(glGenBuffers(1, &m_RendererID));
    GL_CALL(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_RendererID));
    GL_CALL(glBufferData(GL_ELEMENT_ARRAY_BUFFER, count * sizeof(uint32_t), indices, GL_STATIC_DRAW));
}

OpenGLIndexBuffer::~OpenGLIndexBuffer()
{
    OVEN_PROFILE_FUNCTION();
    GL_CALL(glDeleteBuffers(1, &m_RendererID));
}

void OpenGLIndexBuffer::Bind() const
{
    OVEN_PROFILE_FUNCTION();
    GL_CALL(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_RendererID));
}
void OpenGLIndexBuffer::Unbind() const
{
    OVEN_PROFILE_FUNCTION();
    GL_CALL(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0));
}
} // namespace Oven