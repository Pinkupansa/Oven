#pragma once
#include "Oven/Core/Core.h"
#include "Oven/Core/Window.h"
#include "Oven/Core/LayerStack.h"
#include "Oven/Core/Time.h"
#include "Oven/Events/Event.h"
#include "Oven/Events/ApplicationEvent.h"
#include "Oven/ImGui/ImGuiLayer.h"
#include "Oven/Renderer/Shader.h"
#include "Oven/Renderer/Buffer.h"
#include "Oven/Renderer/VertexArray.h"
#include "Oven/Renderer/OrthographicCamera.h"

namespace Oven
{
class OVEN_API Application
{
public:
    Application(const std::string& name = "Oven App");
    virtual ~Application();
    void Run();
    void OnEvent(Event& e);
    void PushLayer(Layer* layer);
    void PushOverlay(Layer* layer);

    inline static Application& Get() { return *s_Instance; }

    inline Window& GetWindow() { return *m_Window; }

    ImGuiLayer* GetImGuiLayer() { return m_ImGuiLayer; }

private:
    bool OnWindowClose(WindowCloseEvent& e);
    bool OnWindowResize(WindowResizeEvent& e);

private:
    std::unique_ptr<Window> m_Window;
    ImGuiLayer* m_ImGuiLayer;
    bool m_Minimized = false;
    bool m_Running = true;
    LayerStack m_LayerStack;

private:
    static Application* s_Instance;
};

// define in client
Application* CreateApplication();
} // namespace Oven