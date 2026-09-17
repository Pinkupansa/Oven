#include "Oven/ovenpch.h"
#include "Oven/Platform/OpenGL/OpenGLUniformBuffer.h"
#include "Oven/Platform/OpenGL/OpenGLMacros.h"
#include "Oven/Platform/OpenGL/OpenGLCaps.h"
#include <glad/glad.h>

namespace Oven
{

OpenGLUniformBuffer::OpenGLUniformBuffer(uint32_t size, uint32_t binding)
{
    GL_CALL(glGenBuffers(1, &m_RendererID));
    GL_CALL(glBindBuffer(GL_UNIFORM_BUFFER, m_RendererID));
    GL_CALL(glBufferData(GL_UNIFORM_BUFFER, size, nullptr, GL_DYNAMIC_DRAW));
    GL_CALL(glBindBuffer(GL_UNIFORM_BUFFER, 0));

    GL_CALL(glBindBufferBase(GL_UNIFORM_BUFFER, binding, m_RendererID));
}

OpenGLUniformBuffer::~OpenGLUniformBuffer() { GL_CALL(glDeleteBuffers(1, &m_RendererID)); }

void OpenGLUniformBuffer::SetData(const void* data, uint32_t size, uint32_t offset)
{
    GL_CALL(glBindBuffer(GL_UNIFORM_BUFFER, m_RendererID));
    GL_CALL(glBufferSubData(GL_UNIFORM_BUFFER, offset, size, data));
    GL_CALL(glBindBuffer(GL_UNIFORM_BUFFER, 0));
}

} // namespace Oven