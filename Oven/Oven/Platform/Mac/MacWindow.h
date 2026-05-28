#pragma once
#include "Oven/Window.h"
class GLFWwindow;
namespace Oven{ 
    class MacWindow : public Window{
        public: 
            MacWindow(const WindowProps& props);
            virtual ~MacWindow();
            void OnUpdate() override;
            inline unsigned int GetWidth() const override { return m_Data.Width; }
            inline unsigned int GetHeight() const override { return m_Data.Height; }

            inline unsigned int GetContentScaleX() const override { return m_Data.ContentScaleX; }
            inline unsigned int GetContentScaleY() const override { return m_Data.ContentScaleY; }
            inline void SetEventCallback(const EventCallbackFn& callback) override { m_Data.EventCallback = callback; }
            
            void SetVSync(bool enabled) override; 
            bool IsVSync() const override;
            inline virtual void* GetNativeWindow() const override {return (void*)m_Window;}
            private: 
                virtual void Init(const WindowProps& props);
                virtual void Shutdown();
                

            private: 
                GLFWwindow* m_Window;
                struct WindowData{
                    std::string Title; 
                    unsigned int Width, Height, ContentScaleX, ContentScaleY; 
                    bool VSync;
                    EventCallbackFn EventCallback;
                };
                WindowData m_Data;
    };
}