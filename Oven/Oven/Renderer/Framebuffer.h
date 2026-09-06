#pragma once
#include "Oven/Core/Core.h"

namespace Oven
{

enum class FramebufferTextureFormat {
    None = 0,
    // Color
    COLOR_UNORM8_RGBA,
    COLOR_UNORM8_RGB,
    COLOR_INT8_R,
    // Depth/Stencil
    DEPTH_UNORM24_STENCIL_UINT8,
};

struct FramebufferTextureSpecs
{
    FramebufferTextureSpecs() = default;
    FramebufferTextureSpecs(FramebufferTextureFormat format) : TextureFormat(format) {}
    FramebufferTextureFormat TextureFormat = FramebufferTextureFormat::None;
};

struct FramebufferAttachmentsSpecs
{
    FramebufferAttachmentsSpecs() = default;
    FramebufferAttachmentsSpecs(std::initializer_list<FramebufferTextureSpecs> textureSpecs)
        : TextureSpecs(textureSpecs) {};
    std::vector<FramebufferTextureSpecs> TextureSpecs;
};
struct FramebufferSpecs
{
    uint32_t Width, Height;
    FramebufferAttachmentsSpecs AttachmentsSpecs;
    uint32_t Samples = 1;
    bool SwapchainTarget = false;
};

class Framebuffer
{
public:
    virtual ~Framebuffer() = default;
    virtual const FramebufferSpecs& GetSpecs() const = 0;
    virtual void Bind() = 0;
    virtual void Unbind() = 0;
    virtual void Resize(uint32_t width, uint32_t height) = 0;
    virtual uint32_t GetColorAttachmentRendererID(uint32_t index = 0) const = 0;
    static Ref<Framebuffer> Create(const FramebufferSpecs& specs);
};
} // namespace Oven
