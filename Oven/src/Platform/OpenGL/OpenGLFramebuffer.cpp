#include "Oven/ovenpch.h"
#include "Oven/Platform/OpenGL/OpenGLFramebuffer.h"
#include "Oven/Platform/OpenGL/OpenGLMacros.h"
#include <glad/glad.h>

namespace Oven
{
static const uint32_t MaxFramebufferSize = 8192;

OpenGLFramebuffer::OpenGLFramebuffer(const FramebufferSpecs& specs) : m_Specs(specs)
{
    Invalidate();
}

OpenGLFramebuffer::~OpenGLFramebuffer()
{
    Clear();
}

void OpenGLFramebuffer::Clear()
{
    GL_CALL(glDeleteFramebuffers(1, &m_RendererID));
    GL_CALL(glDeleteTextures(1, &m_ColorAttachment));
    GL_CALL(glDeleteTextures(1, &m_DepthAttachment));
}

void OpenGLFramebuffer::Invalidate()
{
    if (m_RendererID)
    {
        Clear();
    }
    GL_CALL(glGenFramebuffers(1, &m_RendererID));
    GL_CALL(glBindFramebuffer(GL_FRAMEBUFFER, m_RendererID));

    // --- Color Attachment ---
    GL_CALL(glGenTextures(1, &m_ColorAttachment));
    GL_CALL(glBindTexture(GL_TEXTURE_2D, m_ColorAttachment));
    GL_CALL(
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, m_Specs.Width, m_Specs.Height, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr));

    GL_CALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR));
    GL_CALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR));

    GL_CALL(glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_ColorAttachment, 0));

    // --- Depth / Stencil Attachment ---
    GL_CALL(glGenTextures(1, &m_DepthAttachment));
    GL_CALL(glBindTexture(GL_TEXTURE_2D, m_DepthAttachment));
    GL_CALL(glTexImage2D(GL_TEXTURE_2D,
                         0,
                         GL_DEPTH24_STENCIL8,
                         m_Specs.Width,
                         m_Specs.Height,
                         0,
                         GL_DEPTH_STENCIL,
                         GL_UNSIGNED_INT_24_8,
                         nullptr));

    GL_CALL(glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_TEXTURE_2D, m_DepthAttachment, 0));

    // --- Validation ---
    OVEN_CORE_ASSERT(GL_CALL(glCheckFramebufferStatus(GL_FRAMEBUFFER)) == GL_FRAMEBUFFER_COMPLETE,
                     "Framebuffer incomplete !");

    GL_CALL(glBindFramebuffer(GL_FRAMEBUFFER, 0));
}

void OpenGLFramebuffer::Bind()
{
    GL_CALL(glBindFramebuffer(GL_FRAMEBUFFER, m_RendererID));
    GL_CALL(glViewport(0, 0, m_Specs.Width, m_Specs.Height));
}

void OpenGLFramebuffer::Unbind()
{
    GL_CALL(glBindFramebuffer(GL_FRAMEBUFFER, 0));
}

void OpenGLFramebuffer::Resize(uint32_t width, uint32_t height)
{
    if (width == 0 || height == 0 || width > MaxFramebufferSize || height > MaxFramebufferSize)
    {
        OVEN_CORE_WARN("Invalid framebuffer size : {0} {1}", width, height);
        return;
    }
    m_Specs.Width = width;
    m_Specs.Height = height;
    Invalidate();
}
} // namespace Oven