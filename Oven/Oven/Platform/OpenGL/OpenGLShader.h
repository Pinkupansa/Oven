#pragma once 
#include <string> 
#include "Oven/Renderer/Shader.h"

#include <glad/glad.h>
//REMOVE THIS 
typedef unsigned int GLenum;
namespace Oven{

    class OpenGLShader: public Shader{ 
        public: 
            OpenGLShader(const std::string& filepath);
            OpenGLShader(const std::string& name, const std::string& vertexSrc, const std::string& fragmentSrc);
            ~OpenGLShader();  

            void SetFloat(const std::string& name, const float value) override;
            void SetFloat2(const std::string& name, const glm::vec2& value) override;
            void SetFloat3(const std::string& name, const glm::vec3& value) override;
            void SetFloat4(const std::string& name, const glm::vec4& value) override;
            void SetMat4(const std::string& name, const glm::mat4& value) override;
            void SetInt(const std::string& name, const int value) override;
            void SetIntArray(const std::string& name, int* values, uint32_t count);

            const std::string& GetDefaultName() const override { return m_Name; } 

            void Bind() const override; 
            void Unbind() const override; 
        
        private: 
            std::string ReadFile(const std::string& filepath);
            std::unordered_map<GLenum, std::string> SplitShaderSources(const std::string& source);
            void Compile(const std::unordered_map<GLenum, std::string>& shaderSources);
            inline GLint GetUniformLocationSafe(const std::string& name) const;
            void UploadUniformInt(const std::string& name, int value); 
            void UploadUniformFloat(const std::string& name, float value); 
            void UploadUniformFloat2(const std::string& name, const glm::vec2& values); 
            void UploadUniformFloat3(const std::string& name, const glm::vec3& values); 
            void UploadUniformFloat4(const std::string& name, const glm::vec4& values); 
            void UploadUniformMat3(const std::string& name, const glm::mat3& matrix); 
            void UploadUniformMat4(const std::string& name, const glm::mat4& matrix); 
            void UploadUniformIntArray(const std::string& name, int* values, uint32_t count);
        private: 
            uint32_t m_RendererID;
            std::string m_Name;
    };
}