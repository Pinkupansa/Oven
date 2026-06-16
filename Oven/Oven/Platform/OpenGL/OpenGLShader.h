#pragma once 
#include <string> 
#include "Oven/Renderer/Shader.h"
namespace Oven{

    class OpenGLShader: public Shader{ 
        public: 
            OpenGLShader(const std::string& vertexSrc, const std::string& fragmentSrc);
            ~OpenGLShader(); 

            void UploadUniformMat4(const std::string& name, const glm::mat4& matrix) override; //add type abstraction ? 
            void Bind() const override; 
            void Unbind() const override; 
        
        private: 
            uint32_t m_RendererID;
    };
}