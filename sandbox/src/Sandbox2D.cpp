#include "Sandbox2D.h"
#include "imgui.h"

#include <glm/gtc/matrix_transform.hpp> 
#include <glm/gtc/type_ptr.hpp>

#include <chrono> 
#include <iostream>

#include "Oven/Debug/Instrumentor.h"

Sandbox2D::Sandbox2D() : Layer("Sandbox2D"), m_CameraController(1280.0/720.0){}

void Sandbox2D::OnUpdate()
{
    OVEN_PROFILE_FUNCTION();
    m_CameraController.OnUpdate();
    

    //Render
    {
        OVEN_PROFILE_SCOPE("Renderer Preparation");
        Oven::RenderCommand::SetClearColor({0.1f, 0.1f, 0.1f, 1});
        Oven::RenderCommand::Clear();
    }

    {
        OVEN_PROFILE_SCOPE("Renderer Draw");
        Oven::Renderer2D::BeginScene(m_CameraController.GetCamera());
        Oven::Renderer2D::DrawQuad({-1.0f, 0.0f}, {0.8f, 1.0f}, {0.2f, 0.5f, 0.2f, 1.0f});
        Oven::Renderer2D::DrawQuad({1.0f, 0.5f}, {0.2f, 0.3f}, {0.9f, 0.1f, 0.2f, 1.0f});
        Oven::Renderer2D::DrawRotatedQuad({-2.0f, 0.0f, -0.1f}, {10.0f, 10.0f}, 45.0f,  m_SandTexture, {1.0f, 1.0f, 1.0f, 1.0f}, {10.0F, 10.0F});
        Oven::Renderer2D::EndScene();
    }
}

void Sandbox2D::OnImGuiRender()
{
    OVEN_PROFILE_FUNCTION();
    ImGui::Begin("Profiling");

    ImGui::End();
}

void Sandbox2D::OnAttach()
{
    OVEN_PROFILE_FUNCTION();
    m_SandTexture = Oven::Texture2D::Create("sandbox/assets/textures/sand.png");
}

void Sandbox2D::OnDetach()
{
    OVEN_PROFILE_FUNCTION();
}

void Sandbox2D::OnEvent(Oven::Event &e)
{
m_CameraController.OnEvent(e);
}
