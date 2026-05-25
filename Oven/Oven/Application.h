#pragma once 
#include "Core.h"
namespace Oven{
    class OVEN_API Application{
        public:
            Application();
            virtual ~Application();
            void Run();

    };

    //define in client
    Application* CreateApplication();
}