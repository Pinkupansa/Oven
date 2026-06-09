#pragma once 
#include "Oven/Core.h"
#include "Oven/Window.h"
#include "Oven/LayerStack.h"
#include "Oven/Events/Event.h"
#include "Oven/Events/ApplicationEvent.h"
#include "Oven/ImGui/ImGuiLayer.h"
#include "Oven/Renderer/Shader.h"
#include "Oven/Renderer/Buffer.h"
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

            unsigned int m_VertexArray;
            std::unique_ptr<Shader> m_Shader;
            std::unique_ptr<VertexBuffer> m_VertexBuffer;
            std::unique_ptr<IndexBuffer> m_IndexBuffer;
        private:
            static Application* s_Instance;
    };

    //define in client
    Application* CreateApplication();
}