#pragma once 
#include "Oven/Core.h"
#include "Oven/Window.h"
#include "Oven/LayerStack.h"
#include "Oven/Events/Event.h"
#include "Oven/Events/ApplicationEvent.h"
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
            bool m_Running = true;
            LayerStack m_LayerStack;
        
        private:
            static Application* s_Instance;
    };

    //define in client
    Application* CreateApplication();
}