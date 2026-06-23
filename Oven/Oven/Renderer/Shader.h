#pragma once 
#include <string>
#include <glm/gtc/matrix_transform.hpp>
namespace Oven{

    class Shader{
        public:
            virtual ~Shader() = default;

            virtual void Bind() const = 0; 
            virtual void Unbind() const = 0;
            
            static Shader* Create(const std::string &filepath);
            static Shader* Create(const std::string &vertexSrc, const std::string &fragmentSrc);
    };
}