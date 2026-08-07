#include "Oven/ovenpch.h"
#include "Oven/Renderer/Renderer2D.h"
#include "Oven/Renderer/VertexArray.h"
#include "Oven/Renderer/Shader.h"
#include "Oven/Renderer/RenderCommand.h"
#include "Oven/Platform/OpenGL/OpenGLShader.h"
namespace Oven{

    struct Renderer2DData{
        Ref<VertexArray> QuadVertexArray;
        Ref<Shader> FlatColorShader;
    };

    static Renderer2DData* s_Data;

    void Renderer2D::Init(){
        
        s_Data = new Renderer2DData();
        s_Data->QuadVertexArray = Oven::VertexArray::Create();

        float squareVertices[3*4] = { 
            -0.5f, -0.5f, 0.0f,
            0.5f, -0.5f, 0.0f,
            0.5f, 0.5f, 0.0f,
            -0.5f, 0.5f, 0.0f,
        };
        Ref<VertexBuffer> squareVB;
        squareVB.reset(VertexBuffer::Create(squareVertices, sizeof(squareVertices)));
        squareVB->SetLayout({{Oven::ShaderDataType::Float3, "a_Position"}});
        s_Data->QuadVertexArray->AddVertexBuffer(squareVB);
        uint32_t squareIndices[6] = {0, 1, 2, 3, 3, 0};
        Ref<IndexBuffer> squareIB;
        squareIB.reset(IndexBuffer::Create(squareIndices, sizeof(squareIndices)/sizeof(uint32_t)));
        s_Data->QuadVertexArray->SetIndexBuffer(squareIB);
        s_Data->FlatColorShader = Shader::Create("sandbox/assets/shaders/FlatColor.glsl");

    }

    void Renderer2D::Shutdown(){
        delete s_Data;
    }

    void Renderer2D::BeginScene(const OrthographicCamera& camera){
        s_Data->FlatColorShader->Bind();
        s_Data->FlatColorShader->SetMat4("u_ViewProjection", camera.GetViewProjectionMatrix());
        s_Data->FlatColorShader->SetMat4("u_Model", glm::mat4(1.0f));
    }

    void Renderer2D::EndScene(){

    }

    void Renderer2D::DrawQuad(const glm::vec3& position, const glm::vec2& size, const glm::vec4& color){
        s_Data->FlatColorShader->Bind();
        s_Data->FlatColorShader->SetFloat4("u_Color", color);
        s_Data->QuadVertexArray->Bind(); 
        RenderCommand::DrawIndexed(s_Data->QuadVertexArray);
    }

    void Renderer2D::DrawQuad(const glm::vec2& position, const glm::vec2& size, const glm::vec4& color){
        DrawQuad({position.x, position.y, 0.0f}, size, color);
    }
}