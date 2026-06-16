#pragma once 
#include <string>
#include <glm/gtc/matrix_transform.hpp>
namespace Oven{

    class Shader{
        public:
            virtual void Bind() const = 0; 
            virtual void Unbind() const = 0;
            
            virtual void UploadUniformMat4(const std::string& name, const glm::mat4& matrix) = 0; //add type abstraction ? 

            static Shader* Create(std::string &vertexSrc, std::string &fragmentSrc);
    };

}