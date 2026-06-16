#include "Oven/ovenpch.h"
#include "Oven/Platform/OpenGL/OpenGLVertexArray.h"
#include "Oven/Platform/OpenGL/OpenGLMacros.h"
#include <glad/glad.h>
namespace Oven{

    static GLenum ShaderDataTypeToOpenGLDataType(ShaderDataType type){
        switch (type)
        {
            case ShaderDataType::Float: return GL_FLOAT;
            case ShaderDataType::Float2: return GL_FLOAT;
            case ShaderDataType::Float3: return GL_FLOAT;
            case ShaderDataType::Float4: return GL_FLOAT;
            case ShaderDataType::Mat3: return GL_FLOAT;
            case ShaderDataType::Mat4: return GL_FLOAT;
            case ShaderDataType::Int: return GL_INT;
            case ShaderDataType::Int2: return GL_INT;
            case ShaderDataType::Int3: return GL_INT;
            case ShaderDataType::Int4: return GL_INT;
            case ShaderDataType::Bool: return GL_BOOL;
        }
    }

    OpenGLVertexArray::OpenGLVertexArray(){
        GL_CALL(glGenVertexArrays(1, &m_RendererID));
    }
    void OpenGLVertexArray::Bind() const{
        GL_CALL(glBindVertexArray(m_RendererID));
    }
    void OpenGLVertexArray::Unbind() const{
        GL_CALL(glBindVertexArray(0));
    }

    void OpenGLVertexArray::AddVertexBuffer(const std::shared_ptr<VertexBuffer>& vertexBuffer){
        OVEN_CORE_ASSERT(vertexBuffer->GetLayout().GetElements().size(), "Vertex buffer has no layout !");
        
        glBindVertexArray(m_RendererID);
        vertexBuffer->Bind(); 

        uint32_t index = 0 ;
        const auto& layout = vertexBuffer->GetLayout();
        
        for(const auto& element : layout){
            GL_CALL(glEnableVertexAttribArray(index));
            GL_CALL(glVertexAttribPointer(index, 
                element.GetComponentCount(), 
            ShaderDataTypeToOpenGLDataType(element.Type), 
            element.Normalized ? GL_TRUE : GL_FALSE,
            layout.GetStride(), 
            (const void *)element.Offset));
            index++;
        }
        m_VertexBuffers.push_back(vertexBuffer);
    }
    
    void OpenGLVertexArray::SetIndexBuffer(const std::shared_ptr<IndexBuffer>& indexBuffer){ 
        GL_CALL(glBindVertexArray(m_RendererID));
        indexBuffer->Bind(); 

        m_IndexBuffer = indexBuffer;
    }
}