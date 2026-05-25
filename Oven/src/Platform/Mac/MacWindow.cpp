#include "Oven/Platform/Mac/MacWindow.h"
#include "Oven/Log.h"
namespace Oven{
    static bool s_GLFWInitialized = false;

    Window* Window::Create(const WindowProps& props){
        return new MacWindow(props);
    }

    MacWindow::MacWindow(const WindowProps& props){
        Init(props);
    }

    MacWindow::~MacWindow(){ Shutdown(); }

    void MacWindow::OnUpdate(){
        glfwPollEvents();
        glfwSwapBuffers(m_Window);
    }

    void MacWindow::Shutdown(){
        glfwDestroyWindow(m_Window);
    }

    void MacWindow::Init(const WindowProps& props){
        m_Data.Title = props.Title; 
        m_Data.Width = props.Width;
        m_Data.Height = props.Height; 

        OVEN_CORE_INFO("Creating window {0} ({1}, {2})", props.Title, props.Width, props.Height);

        if(!s_GLFWInitialized){

            int success = glfwInit();
            OVEN_CORE_ASSERT(success, "Could not initialize GLFW!");
            s_GLFWInitialized = true;
        }

        m_Window = glfwCreateWindow((int)props.Width, (int)props.Height, m_Data.Title.c_str(), nullptr, nullptr);
        glfwMakeContextCurrent(m_Window);
        glfwSetWindowUserPointer(m_Window, &m_Data);
        SetVSync(true);
    }

    void MacWindow::SetVSync(bool enabled){
        if(enabled)
            glfwSwapInterval(1);
        else
            glfwSwapInterval(0);
        
        m_Data.VSync = enabled;
    }

    bool MacWindow::IsVSync() const{
        return m_Data.VSync;
    }
}