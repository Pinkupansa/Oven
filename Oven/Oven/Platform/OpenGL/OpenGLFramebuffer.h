#pragma once
#include "Oven/Renderer/Framebuffer.h"
namespace Oven
{
class OpenGLFramebuffer : public Framebuffer
{
public:
    OpenGLFramebuffer(const FramebufferSpecs& specs);
    ~OpenGLFramebuffer() override;

    void Invalidate();
    const FramebufferSpecs& GetSpecs() const override { return m_Specs; }
    void Bind() override;
    void Unbind() override;
    void Resize(uint32_t width, uint32_t height) override;
    uint32_t GetColorAttachmentRendererID() const override { return m_ColorAttachment; }

private:
    uint32_t m_RendererID = 0;
    uint32_t m_ColorAttachment = 0, m_DepthAttachment = 0;
    FramebufferSpecs m_Specs;

private:
    void Clear();
};
} // namespace Oven