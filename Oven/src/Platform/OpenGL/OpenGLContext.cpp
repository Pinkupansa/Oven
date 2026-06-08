#include "Oven/ovenpch.h"
#include "Oven/Platform/OpenGL/OpenGLContext.h"
#include "Oven/Log.h"
#include <glad/glad.h>
#include <GLFW/glfw3.h>
namespace Oven{
    OpenGLContext::OpenGLContext(GLFWwindow* windowHandle) : m_WindowHandle(windowHandle){
        OVEN_CORE_ASSERT(windowHandle, "Window handle does not exist !");
    };

    void OpenGLContext::Init(){
        glfwMakeContextCurrent(m_WindowHandle);
        int status = gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
        OVEN_CORE_ASSERT(status, "Failed to initialize GLAD!");
    }

    void OpenGLContext::SwapBuffers(){
        
        glfwSwapBuffers(m_WindowHandle);
    }

}