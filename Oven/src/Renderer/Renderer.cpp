#include "Oven/ovenpch.h"
#include "Oven/Renderer/Renderer.h"
#include "Oven/Platform/OpenGL/OpenGLShader.h"
namespace Oven
{   
    Renderer::SceneData* Renderer::m_SceneData = new Renderer::SceneData;
    void Renderer::BeginScene(OrthographicCamera& camera){
        m_SceneData->ViewProjectionMatrix = camera.GetViewProjectionMatrix();
    }

    void Renderer::EndScene(){

    }

void Renderer::Submit(const Ref<Shader>& shader, const Ref<VertexArray>& vertexArray, const glm::mat4& transform){
        std::dynamic_pointer_cast<OpenGLShader>(shader)->UploadUniformMat4("u_ViewProjection", m_SceneData->ViewProjectionMatrix);
        std::dynamic_pointer_cast<OpenGLShader>(shader)->UploadUniformMat4("u_Model", transform);
        vertexArray->Bind();
        RenderCommand::DrawIndexed(vertexArray);
    }
} // namespace Oven
