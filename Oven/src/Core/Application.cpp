#include "Oven/ovenpch.h"
#include "Oven/Core/Application.h"
#include "Oven/Core/Log.h"
#include "Oven/Core/Input.h"
#include "Oven/Platform/OpenGL/OpenGLMacros.h"
#include "Oven/Renderer/Renderer.h"
#include <glad/glad.h>
#include "Oven/Core/Time.h"

namespace Oven
{
#define BIND_EVENT_FN(x) std::bind(&Application::x, this, std::placeholders::_1)
Application* Application::s_Instance = nullptr;

Application::Application(const std::string& name)
{
    OVEN_PROFILE_FUNCTION();

    OVEN_CORE_ASSERT(!s_Instance, "Application already exists !");
    s_Instance = this;
    m_Window = std::unique_ptr<Window>(Window::Create(WindowProps(name)));
    m_Window->SetVSync(false);
    m_Window->SetEventCallback(BIND_EVENT_FN(OnEvent));
    m_ImGuiLayer = new ImGuiLayer();
    PushOverlay(m_ImGuiLayer);
    Renderer::Init();
}
Application::~Application() { OVEN_PROFILE_FUNCTION(); }

void Application::PushLayer(Layer* layer)
{
    OVEN_PROFILE_FUNCTION();

    m_LayerStack.PushLayer(layer);
    layer->OnAttach();
}
void Application::PushOverlay(Layer* layer)
{
    OVEN_PROFILE_FUNCTION();

    m_LayerStack.PushOverlay(layer);
    layer->OnAttach();
}
void Application::Run()
{
    OVEN_PROFILE_FUNCTION();

    while (m_Running)
    {
        OVEN_PROFILE_SCOPE("Run Loop");
        Time::OnUpdate();
        // Update layers
        if (!m_Minimized)
        {
            {
                OVEN_PROFILE_SCOPE("LayerStack OnUpdate");
                for (Layer* layer : m_LayerStack)
                    layer->OnUpdate();
            }
            m_ImGuiLayer->Begin();
            {
                OVEN_PROFILE_SCOPE("LayerStack OnImGuiRender");
                for (Layer* layer : m_LayerStack)
                    layer->OnImGuiRender();
                m_ImGuiLayer->End();
            }
            m_Window->OnUpdate();
        }
    }
}

void Application::OnEvent(Event& e)
{
    OVEN_PROFILE_FUNCTION();

    EventDispatcher dispatcher(e);
    dispatcher.Dispatch<WindowCloseEvent>(BIND_EVENT_FN(OnWindowClose));
    dispatcher.Dispatch<WindowResizeEvent>(BIND_EVENT_FN(OnWindowResize));
    // Backwards event propagation
    for (auto it = m_LayerStack.end(); it != m_LayerStack.begin();)
    {
        if (e.Handled())
            break;
        (*--it)->OnEvent(e);
    }
}

bool Application::OnWindowClose(WindowCloseEvent& e)
{
    m_Running = false;
    return true;
}

bool Application::OnWindowResize(WindowResizeEvent& e)
{
    OVEN_PROFILE_FUNCTION();

    if (e.GetWidth() == 0 || e.GetHeight() == 0)
    {
        m_Minimized = true;
        return false;
    }
    m_Minimized = false;
    Renderer::OnWindowResize(e.GetWidth(), e.GetHeight());

    return false;
}
} // namespace Oven
