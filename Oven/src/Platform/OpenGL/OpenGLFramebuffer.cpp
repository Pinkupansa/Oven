#include "Oven/ovenpch.h"
#include "Oven/Platform/OpenGL/OpenGLFramebuffer.h"
#include "Oven/Platform/OpenGL/OpenGLMacros.h"
#include <glad/glad.h>

namespace Oven
{
static const uint32_t MaxFramebufferSize = 8192;
static bool IsDepthFormat(FramebufferTextureFormat format)
{
    switch (format)
    {
        case FramebufferTextureFormat::DEPTH_UNORM24_STENCIL_UINT8:
            return true;
        default:
            break;
    }

    return false;
}

static inline GLenum TextureTarget(bool multisampled)
{ return multisampled ? GL_TEXTURE_2D_MULTISAMPLE : GL_TEXTURE_2D; }

static void CreateFramebufferTextures(uint32_t* outID, uint32_t count) { GL_CALL(glGenTextures(count, outID)); }

static void BindFramebufferTexture(int nSample, uint32_t id)
{
    bool multisampled = nSample > 1;
    GL_CALL(glBindTexture(TextureTarget(multisampled), id));
}
static void AttachColorTexture(
    uint32_t id,
    int samples,
    GLenum internalFormat,
    GLenum format,
    GLenum dataType,
    uint32_t width,
    uint32_t height,
    int index
)
{
    bool multisampled = samples > 1;
    if (multisampled)
    {
        GL_CALL(glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, samples, internalFormat, width, height, GL_FALSE));
    }
    else
    {
        GL_CALL(glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, width, height, 0, format, dataType, nullptr));
        GL_CALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR));
        GL_CALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR));
        GL_CALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE));
        GL_CALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE));
    }
    GL_CALL(glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + index, TextureTarget(multisampled), id, 0));
}
static void AttachDepthTexture(
    uint32_t id, int samples, GLenum format, GLenum attachmentType, GLenum dataType, uint32_t width, uint32_t height
)
{
    bool multisampled = samples > 1;
    if (multisampled)
    {
        GL_CALL(glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, samples, format, width, height, GL_FALSE));
    }
    else
    {
        GL_CALL(glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, GL_DEPTH_STENCIL, dataType, nullptr));
        GL_CALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR));
        GL_CALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR));
        GL_CALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE));
        GL_CALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE));
    }
    GL_CALL(glFramebufferTexture2D(GL_FRAMEBUFFER, attachmentType, TextureTarget(multisampled), id, 0));
}

OpenGLFramebuffer::OpenGLFramebuffer(const FramebufferSpecs& specs) : m_Specs(specs)
{
    GL_CALL(glDisable(GL_FRAMEBUFFER_SRGB));
    for (auto texSpecs : m_Specs.AttachmentsSpecs.TextureSpecs)
    {
        if (IsDepthFormat(texSpecs.TextureFormat))
            m_DepthAttachmentSpecs = texSpecs;
        else
            m_ColorAttachmentsSpecs.emplace_back(texSpecs);
    }

    Invalidate();
}

OpenGLFramebuffer::~OpenGLFramebuffer() { Clear(); }

void OpenGLFramebuffer::Clear()
{

    GL_CALL(glDeleteFramebuffers(1, &m_RendererID));
    if (m_DepthAttachment)
    {
        GL_CALL(glDeleteTextures(1, &m_DepthAttachment));
        m_DepthAttachment = 0;
    }

    if (!m_ColorAttachments.empty())
    {
        GL_CALL(glDeleteTextures((GLsizei)m_ColorAttachments.size(), m_ColorAttachments.data()));
        m_ColorAttachments.clear();
    }
}

void OpenGLFramebuffer::Invalidate()
{
    if (m_RendererID)
        Clear();

    GL_CALL(glGenFramebuffers(1, &m_RendererID));
    GL_CALL(glBindFramebuffer(GL_FRAMEBUFFER, m_RendererID));

    // Attachments
    // --- Color Attachments ---

    if (m_ColorAttachmentsSpecs.size())
    {
        m_ColorAttachments.resize(m_ColorAttachmentsSpecs.size());
        CreateFramebufferTextures(m_ColorAttachments.data(), m_ColorAttachments.size());
        for (size_t i = 0; i < m_ColorAttachments.size(); i++)
        {
            BindFramebufferTexture(m_Specs.Samples, m_ColorAttachments[i]);
            switch (m_ColorAttachmentsSpecs[i].TextureFormat)
            {
                case FramebufferTextureFormat::COLOR_UNORM8_RGBA:
                    AttachColorTexture(
                        m_ColorAttachments[i],
                        m_Specs.Samples,
                        GL_RGBA8,
                        GL_RGBA,
                        GL_UNSIGNED_BYTE,
                        m_Specs.Width,
                        m_Specs.Height,
                        i
                    );
                    break;
                case FramebufferTextureFormat::COLOR_INT8_R:
                    AttachColorTexture(
                        m_ColorAttachments[i],
                        m_Specs.Samples,
                        GL_R32I,
                        GL_RED_INTEGER,
                        GL_BYTE,
                        m_Specs.Width,
                        m_Specs.Height,
                        i
                    );
                    break;
                default:
                    OVEN_ASSERT(false, "Currently unsupported color FramebufferTextureFormat !");
                    break;
            }
        }
    }

    // --- Depth / Stencil Attachment ---

    if (m_DepthAttachmentSpecs.TextureFormat != FramebufferTextureFormat::None)
    {
        CreateFramebufferTextures(&m_DepthAttachment, 1);
        BindFramebufferTexture(m_Specs.Samples, m_DepthAttachment);

        switch (m_DepthAttachmentSpecs.TextureFormat)
        {
            case FramebufferTextureFormat::DEPTH_UNORM24_STENCIL_UINT8:
                AttachDepthTexture(
                    m_DepthAttachment,
                    m_Specs.Samples,
                    GL_DEPTH24_STENCIL8,
                    GL_DEPTH_STENCIL_ATTACHMENT,
                    GL_UNSIGNED_INT_24_8,
                    m_Specs.Width,
                    m_Specs.Height
                );
                break;

            default:
                break;
        }
    }

    if (m_ColorAttachments.size() > 1)
    {
        OVEN_CORE_ASSERT(m_ColorAttachments.size() <= 4, "Too many color attachments !");
        GLenum buffers[4] = {GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2, GL_COLOR_ATTACHMENT3};
        GL_CALL(glDrawBuffers(m_ColorAttachments.size(), buffers)); // "binds" the attachments
    }
    else if (m_ColorAttachments.empty())
    {
        // Only depth-pass
        GL_CALL(glDrawBuffer(GL_NONE));
    }
    // --- Validation ---
    OVEN_CORE_ASSERT(
        GL_CALL(glCheckFramebufferStatus(GL_FRAMEBUFFER)) == GL_FRAMEBUFFER_COMPLETE, "Framebuffer incomplete !"
    );

    GL_CALL(glBindFramebuffer(GL_FRAMEBUFFER, 0));
}

void OpenGLFramebuffer::Bind()
{
    GL_CALL(glBindFramebuffer(GL_FRAMEBUFFER, m_RendererID));
    GL_CALL(glViewport(0, 0, m_Specs.Width, m_Specs.Height));
}

void OpenGLFramebuffer::Unbind() { GL_CALL(glBindFramebuffer(GL_FRAMEBUFFER, 0)); }

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
int OpenGLFramebuffer::ReadPixel(uint32_t attachmentIndex, int x, int y)
{
    OVEN_CORE_ASSERT("Attachment Index out of bounds !", attachmentIndex < m_ColorAttachments.size());
    GL_CALL(glReadBuffer(GL_COLOR_ATTACHMENT0 + attachmentIndex));
    int pixelData;
    GL_CALL(glReadPixels(x, y, 1, 1, GL_RED_INTEGER, GL_INT, &pixelData));
    return pixelData;
}
} // namespace Oven