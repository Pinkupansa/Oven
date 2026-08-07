#include "Sandbox2D.h"
#include "imgui.h"

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
    
    
    Oven::Renderer2D::BeginScene(m_CameraController.GetCamera());
    
    Oven::Renderer2D::DrawQuad({-1.0f, 0.0f}, {0.8f, 1.0f}, {0.2f, 0.5f, 0.2f, 1.0f});
    Oven::Renderer2D::DrawQuad({1.0f, 0.5f}, {0.2f, 0.3f}, {0.9f, 0.1f, 0.2f, 1.0f});
    Oven::Renderer2D::DrawQuad({0.0f, 0.0f, -0.1f}, {10.0f, 10.0f}, m_WaterTexture, {1.0f, 0.7f, 1.0f, 1.0f});
    Oven::Renderer2D::EndScene();

}

void Sandbox2D::OnImGuiRender()
{
    ImGui::Begin("Settings");
    ImGui::End();
}

void Sandbox2D::OnAttach()
{
    m_WaterTexture = Oven::Texture2D::Create("sandbox/assets/textures/water.png");
}

void Sandbox2D::OnDetach()
{
}

void Sandbox2D::OnEvent(Oven::Event &e)
{
m_CameraController.OnEvent(e);
}
