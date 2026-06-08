#pragma once

#include "Oven/Renderer/RenderingContext.h"

struct GLFWwindow;
namespace Oven{
    class OpenGLContext: public RenderingContext{
        public:
            OpenGLContext(GLFWwindow* windowHandle);

            void Init() override; 
            void SwapBuffers() override;
        private: 
            GLFWwindow* m_WindowHandle;
    };
}