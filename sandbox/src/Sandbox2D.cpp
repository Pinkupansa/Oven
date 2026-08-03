#include "Sandbox2D.h"
#include "imgui.h"

#include "Oven/Platform/OpenGL/OpenGLShader.h"
#include <glm/gtc/matrix_transform.hpp> 
#include <glm/gtc/type_ptr.hpp>

Sandbox2D::Sandbox2D() : Layer("Sandbox2D"), m_CameraController(1280.0/720.0){}

void Sandbox2D::OnUpdate()
{
    // Update
    m_CameraController.OnUpdate();

    //Render
    Oven::RenderCommand::SetClearColor({0.1f, 0.1f, 0.1f, 1});
    Oven::RenderCommand::Clear();
    
    
    Oven::Renderer::BeginScene(m_CameraController.GetCamera());
 
    m_FlatColorShader->Bind();
    std::dynamic_pointer_cast<Oven::OpenGLShader>(m_FlatColorShader)->UploadUniformFloat4("u_Color", m_SquareColor);
    Oven::Renderer::Submit(m_FlatColorShader, m_SquareVA, glm::scale(glm::mat4(1.0f), glm::vec3(1.5f)));
    Oven::Renderer::EndScene();
}

void Sandbox2D::OnImGuiRender()
{
    ImGui::Begin("Settings");
    ImGui::ColorEdit4("Square Color", glm::value_ptr(m_SquareColor));
    ImGui::End();
}

void Sandbox2D::OnAttach()
{

    m_SquareVA = Oven::VertexArray::Create();
    float squareVertices[3*4] = { 
        -0.5f, -0.5f, 0.0f,
        0.5f, -0.5f, 0.0f,
        0.5f, 0.5f, 0.0f,
        -0.5f, 0.5f, 0.0f,
    };
    
    Oven::Ref<Oven::VertexBuffer> squareVB;
    squareVB.reset(Oven::VertexBuffer::Create(squareVertices, sizeof(squareVertices)));
    
    squareVB->SetLayout({
        {Oven::ShaderDataType::Float3, "a_Position"},
    });

    uint32_t squareIndices[6] = {0, 1, 2, 2, 3, 0};
    
    Oven::Ref<Oven::IndexBuffer> squareIB; 
    squareIB.reset(Oven::IndexBuffer::Create(squareIndices, sizeof(squareIndices)/sizeof(uint32_t)));
    m_SquareVA->AddVertexBuffer(squareVB);
    m_SquareVA->SetIndexBuffer(squareIB);

    m_FlatColorShader = Oven::Shader::Create("sandbox/assets/shaders/FlatColor.glsl"); 
}

void Sandbox2D::OnDetach()
{
}

void Sandbox2D::OnEvent(Oven::Event &e)
{
m_CameraController.OnEvent(e);
}
