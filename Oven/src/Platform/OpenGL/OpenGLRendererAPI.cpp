#include "Oven/ovenpch.h"
#include "Oven/Platform/OpenGL/OpenGLRendererAPI.h"
#include "Oven/Platform/OpenGL/OpenGLMacros.h"

#include <glad/glad.h>
namespace Oven{
    void OpenGLRendererAPI::Init(){
        GL_CALL(glEnable(GL_BLEND));
        GL_CALL(glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA));
        GL_CALL(glEnable(GL_FRAMEBUFFER_SRGB));
    }
    void OpenGLRendererAPI::SetViewport(uint32_t x, uint32_t y, uint32_t width, uint32_t height){
        GL_CALL(glViewport(x, y, width, height));
    }
    void OpenGLRendererAPI::SetClearColor(const glm::vec4& color){
        GL_CALL(glClearColor(color.r, color.g, color.b, color.a));
    }

    void OpenGLRendererAPI::Clear(){
        GL_CALL(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));
    }

    void OpenGLRendererAPI::DrawIndexed(const Ref<VertexArray>& vertexArray){
        GL_CALL(glDrawElements(GL_TRIANGLES, vertexArray->GetIndexBuffer()->GetCount(), GL_UNSIGNED_INT, nullptr));
    }
}