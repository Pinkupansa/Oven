#pragma once 
#include <string>
namespace Oven{

    class Shader{
        public:
            virtual void Bind() const = 0; 
            virtual void Unbind() const = 0; 

            static Shader* Create(std::string &vertexSrc, std::string &fragmentSrc);
    };

}