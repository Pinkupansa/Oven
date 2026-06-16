#pragma once

#include "Oven/Renderer/RendererAPI.h"
#include "Oven/Renderer/RenderCommand.h"
#include "Oven/Renderer/OrthographicCamera.h"
#include "Oven/Renderer/Shader.h"
namespace Oven{ 

    class Renderer{

        public:
            static void BeginScene(OrthographicCamera& camera);
            static void EndScene();

            static void Submit(const std::shared_ptr<Shader>& shader, const std::shared_ptr<VertexArray>& vertexArray);

            inline static RendererAPI::GraphicsAPI GetRenderingAPI(){return RendererAPI::GetAPI();}
        
        private:
            struct SceneData{
                glm::mat4 ViewProjectionMatrix;
            };
            static SceneData* m_SceneData;
    };
}