#include "Oven/ovenpch.h"
#include "Oven/Platform/OpenGL/OpenGLRendererAPI.h"
#include "Oven/Platform/OpenGL/OpenGLMacros.h"

#include <glad/glad.h>
namespace Oven{
    void OpenGLRendererAPI::SetClearColor(const glm::vec4& color){
        GL_CALL(glClearColor(color.r, color.g, color.b, color.a));
    }

    void OpenGLRendererAPI::Clear(){
        GL_CALL(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));
    }

    void OpenGLRendererAPI::DrawIndexed(const std::shared_ptr<VertexArray>& vertexArray){
        GL_CALL(glDrawElements(GL_TRIANGLES, vertexArray->GetIndexBuffer()->GetCount(), GL_UNSIGNED_INT, nullptr));
    }
}