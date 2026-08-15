#pragma once
#include "Oven/Core/Window.h"
class GLFWwindow;
namespace Oven
{
class WindowsWindow : public Window
{
public:
    WindowsWindow(const WindowProps& props);
    virtual ~WindowsWindow();

    void OnUpdate() override;
    inline uint32_t GetWidth() const override { return m_Data.Width; }
    inline uint32_t GetHeight() const override { return m_Data.Height; }

    inline void SetEventCallback(const EventCallbackFn& callback) override { m_Data.EventCallback = callback; }
    inline virtual void* GetNativeWindow() const override { return (void*)m_Window; }
    void SetVSync(bool enabled) override;
    bool IsVSync() const override;

private:
    virtual void Init(const WindowProps& props);
    virtual void Shutdown();

private:
    GLFWwindow* m_Window;
    struct WindowData
    {
        std::string Title;
        uint32_t Width, Height;
        bool VSync;
        EventCallbackFn EventCallback;
    };
    WindowData m_Data;
};
} // namespace Oven
