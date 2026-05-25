#pragma once 
#include "Oven/Core.h"
#include "Oven/Window.h"
namespace Oven{
    class OVEN_API Application{
        public:
            Application();
            virtual ~Application();
            void Run();
        private: 
            std::unique_ptr<Window> m_Window; 
            bool m_Running = true;
    };

    //define in client
    Application* CreateApplication();
}