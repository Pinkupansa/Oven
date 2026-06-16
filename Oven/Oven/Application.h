#pragma once 
#include "Oven/Core.h"
#include "Oven/Window.h"
#include "Oven/LayerStack.h"
#include "Oven/Events/Event.h"
#include "Oven/Events/ApplicationEvent.h"
#include "Oven/ImGui/ImGuiLayer.h"
#include "Oven/Renderer/Shader.h"
#include "Oven/Renderer/Buffer.h"
#include "Oven/Renderer/VertexArray.h"
#include "Oven/Renderer/OrthographicCamera.h"
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

            std::shared_ptr<Shader> m_Shader;
            std::shared_ptr<Shader> m_Shader2;
            std::shared_ptr<VertexArray> m_VertexArray;

            std::shared_ptr<VertexArray> m_SquareVA;
            OrthographicCamera m_Camera;
        private:
            static Application* s_Instance;
    };

    //define in client
    Application* CreateApplication();
}