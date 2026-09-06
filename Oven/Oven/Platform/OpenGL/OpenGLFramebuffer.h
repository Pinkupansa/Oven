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
    uint32_t GetColorAttachmentRendererID(uint32_t index = 0) const override
    {
        OVEN_CORE_ASSERT(index < m_ColorAttachments.size(), "Invalid color attachment index");
        return m_ColorAttachments[index];
    }

private:
    uint32_t m_RendererID = 0;
    FramebufferSpecs m_Specs;

    std::vector<FramebufferTextureSpecs> m_ColorAttachmentsSpecs;
    FramebufferTextureSpecs m_DepthAttachmentSpecs;

    std::vector<uint32_t> m_ColorAttachments;
    uint32_t m_DepthAttachment;

private:
    void Clear();
};
} // namespace Oven