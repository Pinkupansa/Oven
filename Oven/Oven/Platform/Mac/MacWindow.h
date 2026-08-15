#pragma once
#include "Oven/Renderer/RenderingContext.h"
#include "Oven/Core/Window.h"
#include <GLFW/glfw3.h>

namespace Oven
{
class MacWindow : public Window
{
public:
    MacWindow(const WindowProps& props);
    virtual ~MacWindow();
    void OnUpdate() override;
    inline uint32_t GetWidth() const override { return m_Data.Width; }
    inline uint32_t GetHeight() const override { return m_Data.Height; }

    inline uint32_t GetContentScaleX() const override { return m_Data.ContentScaleX; }
    inline uint32_t GetContentScaleY() const override { return m_Data.ContentScaleY; }
    inline void SetEventCallback(const EventCallbackFn& callback) override { m_Data.EventCallback = callback; }

    void SetVSync(bool enabled) override;
    bool IsVSync() const override;
    inline virtual void* GetNativeWindow() const override { return (void*)m_Window; }

private:
    virtual void Init(const WindowProps& props);
    virtual void Shutdown();

private:
    GLFWwindow* m_Window;
    RenderingContext* m_Context;
    struct WindowData
    {
        std::string Title;
        uint32_t Width, Height, ContentScaleX, ContentScaleY;
        bool VSync;
        EventCallbackFn EventCallback;
    };
    WindowData m_Data;
};
} // namespace Oven