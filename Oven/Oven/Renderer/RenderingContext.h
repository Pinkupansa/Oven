#pragma once

namespace Oven
{

class RenderingContext
{
public:
    virtual ~RenderingContext() = default;
    virtual void Init() = 0;
    virtual void SwapBuffers() = 0;
};
} // namespace Oven