#include "Oven/Application.h"
#include "Oven/Log.h"
#include "Oven/Events/ApplicationEvent.h"
namespace Oven{
    Application::Application(){}
    Application::~Application(){}
    void Application::Run(){
        WindowResizeEvent e(1200, 700);
        OVEN_TRACE(e);
        while(true);
    }
}