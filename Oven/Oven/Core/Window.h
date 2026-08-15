#pragma once

#include "Oven/Core/Core.h"
#include "Oven/Events/Event.h"
#include <functional>
namespace Oven
{
struct WindowProps
{
    std::string Title;
    uint32_t Width;
    uint32_t Height;
    WindowProps(const std::string& title = "Oven Engine", uint32_t width = 1200, uint32_t height = 720)
        : Title(title), Width(width), Height(height)
    {
    }
};

class OVEN_API Window
{
public:
    using EventCallbackFn = std::function<void(Event&)>;
    virtual ~Window() {}
    virtual void OnUpdate() = 0;
    virtual uint32_t GetWidth() const = 0;
    virtual uint32_t GetHeight() const = 0;
    virtual inline uint32_t GetContentScaleX() const { return 1; } // overriden in mac window
    virtual inline uint32_t GetContentScaleY() const { return 1; } // overriden in mac window
    // Window attributes
    virtual void SetEventCallback(const EventCallbackFn& callback) = 0;
    virtual void SetVSync(bool enabled) = 0;
    virtual bool IsVSync() const = 0;

    virtual void* GetNativeWindow() const = 0;
    static Scope<Window> Create(const WindowProps& props = WindowProps());
};
} // namespace Oven