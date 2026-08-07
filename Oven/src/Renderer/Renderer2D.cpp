#include "Oven/ovenpch.h"
#include "Oven/Renderer/Renderer2D.h"
#include "Oven/Renderer/VertexArray.h"
#include "Oven/Renderer/Shader.h"
#include "Oven/Renderer/RenderCommand.h"
#include "Oven/Platform/OpenGL/OpenGLShader.h"
#include <glm/gtc/matrix_transform.hpp>
namespace Oven{

    struct Renderer2DData{
        Ref<VertexArray> QuadVertexArray;
        Ref<Shader> TextureShader;
        Ref<Texture2D> WhiteTexture;
        GLint TextureSlot; 
    };

    static Renderer2DData* s_Data;

    void Renderer2D::Init(){
        
        s_Data = new Renderer2DData();
        s_Data->QuadVertexArray = Oven::VertexArray::Create();

        float squareVertices[5*4] = { 
            -0.5f, -0.5f, 0.0f, 0.0f, 0.0f,
            0.5f, -0.5f, 0.0f, 1.0f, 0.0f,
            0.5f, 0.5f, 0.0f, 1.0f, 1.0f,
            -0.5f, 0.5f, 0.0f, 0.0f, 1.0f,
        };
        Ref<VertexBuffer> squareVB;
        squareVB.reset(VertexBuffer::Create(squareVertices, sizeof(squareVertices)));
        squareVB->SetLayout({{Oven::ShaderDataType::Float3, "a_Position"}, {Oven::ShaderDataType::Float2, "a_TexUV"}});
        s_Data->QuadVertexArray->AddVertexBuffer(squareVB);
        uint32_t squareIndices[6] = {0, 1, 2, 3, 2, 0};
        Ref<IndexBuffer> squareIB;
        squareIB.reset(IndexBuffer::Create(squareIndices, sizeof(squareIndices)/sizeof(uint32_t)));
        s_Data->QuadVertexArray->SetIndexBuffer(squareIB);
        s_Data->TextureShader = Shader::Create("sandbox/assets/shaders/Texture.glsl");
        s_Data->TextureSlot = 0; 
        s_Data->TextureShader->Bind();
        s_Data->TextureShader->SetInt("u_Texture", s_Data->TextureSlot);

        s_Data->WhiteTexture = Texture2D::Create(1, 1);
        uint32_t whiteTexData = 0xffffffff;
        s_Data->WhiteTexture->SetData(&whiteTexData, sizeof(uint32_t));

        
    }

    void Renderer2D::Shutdown(){
        delete s_Data;
    }

    void Renderer2D::BeginScene(const OrthographicCamera& camera){
        s_Data->TextureShader->Bind();
        s_Data->TextureShader->SetMat4("u_ViewProjection", camera.GetViewProjectionMatrix());
    }

    void Renderer2D::EndScene(){

    }
    void Renderer2D::DrawQuad(const glm::vec2 &position, const glm::vec2 &size, const glm::vec4 &color)
    {
        DrawQuad({position.x, position.y, 0.0f}, size, s_Data->WhiteTexture, color);
    }
    void Renderer2D::DrawQuad(const glm::vec3 &position, const glm::vec2 &size, const glm::vec4 &color)
    {
        DrawQuad(position, size, s_Data->WhiteTexture, color);
    }
    void Renderer2D::DrawQuad(const glm::vec2 &position, const glm::vec2 &size, const Ref<Texture2D> &texture, const glm::vec4 &color)
    {
        DrawQuad({position.x, position.y, 0.0f}, size, texture, color);
    }
    void Renderer2D::DrawQuad(const glm::vec3 &position, const glm::vec2 &size, const Ref<Texture2D> &texture, const glm::vec4 &color)
    { 
        s_Data->TextureShader->SetFloat4("u_Color", color);
        glm::mat4 transform = glm::translate(glm::mat4(1.0f), position) * glm::scale(glm::mat4(1.0f), {size.x, size.y, 1.0f});
        s_Data->TextureShader->SetMat4("u_Model", transform);
        texture->Bind(s_Data->TextureSlot); 
        s_Data->QuadVertexArray->Bind(); 
        RenderCommand::DrawIndexed(s_Data->QuadVertexArray);
    }
}