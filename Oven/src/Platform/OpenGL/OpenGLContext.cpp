#include "Oven/ovenpch.h"
#include "Oven/Platform/OpenGL/OpenGLContext.h"
#include "Oven/Platform/OpenGL/OpenGLMacros.h"
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

        OVEN_CORE_INFO("OpenGL Version: {0}", (const char*)glGetString(GL_VERSION));
        OVEN_CORE_INFO("Renderer: {0}", (const char*)glGetString(GL_RENDERER));
        OVEN_CORE_INFO("Vendor: {0}", (const char*) glGetString(GL_VENDOR));
        
    }

    void OpenGLContext::SwapBuffers(){
        glfwSwapBuffers(m_WindowHandle);
    }

}