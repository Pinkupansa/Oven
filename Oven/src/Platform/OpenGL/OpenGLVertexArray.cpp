#include "Oven/ovenpch.h"
#include "Oven/Platform/OpenGL/OpenGLVertexArray.h"
#include "Oven/Platform/OpenGL/OpenGLMacros.h"
#include <glad/glad.h>
namespace Oven
{

static GLenum ShaderDataTypeToOpenGLDataType(ShaderDataType type)
{
    switch (type)
    {
        case ShaderDataType::Float:
            return GL_FLOAT;
        case ShaderDataType::Float2:
            return GL_FLOAT;
        case ShaderDataType::Float3:
            return GL_FLOAT;
        case ShaderDataType::Float4:
            return GL_FLOAT;
        case ShaderDataType::Mat3:
            return GL_FLOAT;
        case ShaderDataType::Mat4:
            return GL_FLOAT;
        case ShaderDataType::Int:
            return GL_INT;
        case ShaderDataType::Int2:
            return GL_INT;
        case ShaderDataType::Int3:
            return GL_INT;
        case ShaderDataType::Int4:
            return GL_INT;
        case ShaderDataType::Bool:
            return GL_BOOL;
        case ShaderDataType::None:
            OVEN_CORE_WARN("ShaderDataType is None !");
            return GL_NONE;
    }
}

OpenGLVertexArray::OpenGLVertexArray()
{
    OVEN_PROFILE_FUNCTION();
    GL_CALL(glGenVertexArrays(1, &m_RendererID));
}
void OpenGLVertexArray::Bind() const
{
    OVEN_PROFILE_FUNCTION();
    GL_CALL(glBindVertexArray(m_RendererID));
}
void OpenGLVertexArray::Unbind() const
{
    OVEN_PROFILE_FUNCTION();
    GL_CALL(glBindVertexArray(0));
}
void OpenGLVertexArray::AddVertexBuffer(const Ref<VertexBuffer>& vertexBuffer)
{
    OVEN_PROFILE_FUNCTION();
    OVEN_CORE_ASSERT(vertexBuffer->GetLayout().GetElements().size(), "Vertex buffer has no layout !");

    GL_CALL(glBindVertexArray(m_RendererID));
    vertexBuffer->Bind();

    const auto& layout = vertexBuffer->GetLayout();

    for (const auto& element : layout)
    {
        switch (element.Type)
        {
            case ShaderDataType::Float:
            case ShaderDataType::Float2:
            case ShaderDataType::Float3:
            case ShaderDataType::Float4: {
                GL_CALL(glEnableVertexAttribArray(m_VertexBufferIndex));
                GL_CALL(glVertexAttribPointer(
                    m_VertexBufferIndex,
                    element.GetComponentCount(),
                    ShaderDataTypeToOpenGLDataType(element.Type),
                    element.Normalized ? GL_TRUE : GL_FALSE,
                    layout.GetStride(),
                    reinterpret_cast<const void*>(static_cast<uintptr_t>(element.Offset))
                ));
                m_VertexBufferIndex++;
                break;
            }
            case ShaderDataType::Int:
            case ShaderDataType::Int2:
            case ShaderDataType::Int3:
            case ShaderDataType::Int4:
            case ShaderDataType::Bool: {
                GL_CALL(glEnableVertexAttribArray(m_VertexBufferIndex));
                GL_CALL(glVertexAttribIPointer( // Note le 'I' pour les entiers
                    m_VertexBufferIndex,
                    element.GetComponentCount(),
                    ShaderDataTypeToOpenGLDataType(element.Type),
                    layout.GetStride(),
                    reinterpret_cast<const void*>(static_cast<uintptr_t>(element.Offset))
                ));
                m_VertexBufferIndex++;
                break;
            }
            case ShaderDataType::Mat3:
            case ShaderDataType::Mat4: {
                uint8_t count = element.GetComponentCount();
                for (uint8_t i = 0; i < count; i++)
                {
                    GL_CALL(glEnableVertexAttribArray(m_VertexBufferIndex));
                    GL_CALL(glVertexAttribPointer(
                        m_VertexBufferIndex,
                        count,
                        ShaderDataTypeToOpenGLDataType(element.Type),
                        element.Normalized ? GL_TRUE : GL_FALSE,
                        layout.GetStride(),
                        reinterpret_cast<const void*>(
                            static_cast<uintptr_t>(element.Offset + sizeof(float) * count * i)
                        )
                    ));
                    GL_CALL(glVertexAttribDivisor(m_VertexBufferIndex, 1));
                    m_VertexBufferIndex++;
                }
                break;
            }
            default:
                OVEN_CORE_ASSERT(false, "Unknown ShaderDataType!");
        }
    }

    m_VertexBuffers.push_back(vertexBuffer);
}

void OpenGLVertexArray::SetIndexBuffer(const Ref<IndexBuffer>& indexBuffer)
{
    OVEN_PROFILE_FUNCTION();
    GL_CALL(glBindVertexArray(m_RendererID));
    indexBuffer->Bind();

    m_IndexBuffer = indexBuffer;
}
} // namespace Oven