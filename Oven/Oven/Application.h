#pragma once 
#include "Oven/Core.h"
#include "Oven/Window.h"
#include "Oven/LayerStack.h"
#include "Oven/Events/Event.h"
#include "Oven/Events/ApplicationEvent.h"
#include "Oven/ImGui/ImGuiLayer.h"
#include "Oven/Renderer/Shader.h"

namespace Oven{
    class OVEN_API Application{
        public:
            Application();
            virtual ~Application();
            void Run();
            void OnEvent(Event& e);
            void PushLayer(Layer* layer);
            void PushOverlay(Layer* layer);
            inline static Application& Get() { return *s_Instance; }
            inline Window& GetWindow() { return *m_Window; }
        private: 
            bool OnWindowClose(WindowCloseEvent& e);
            std::unique_ptr<Window> m_Window; 
            ImGuiLayer* m_ImGuiLayer;
            bool m_Running = true;
            LayerStack m_LayerStack;

            unsigned int m_VertexArray, m_VertexBuffer, m_IndexBuffer;
            std::unique_ptr<Shader> m_Shader;
        private:
            static Application* s_Instance;
    };

    //define in client
    Application* CreateApplication();
}