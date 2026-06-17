#pragma once 
#include <glm/glm.hpp>
#include "Oven/Renderer/VertexArray.h"

namespace Oven
{
    
    class RendererAPI{
        public:
            enum class RenderingBackend{

                None = 0, OpenGL = 1
            };
        public:
            virtual void SetClearColor(const glm::vec4& color) = 0; 
            virtual void Clear() = 0;
            virtual void DrawIndexed(const std::shared_ptr<VertexArray>& vertexArray) = 0;
            inline static RenderingBackend GetBackend() { return s_RenderingBackend; }
        
        private:
            static RenderingBackend s_RenderingBackend;
    };
} // namespace Oven
