#include "Sandbox2D.h"
#include "imgui.h"
#include "Oven/Core/Time.h"
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
        //Oven::Renderer2D::DrawQuad({0.0f, 0.0f, -0.1f}, {20.0f, 20.0f}, m_SpriteSheet, {1.0f, 1.0f, 1.0f, 1.0f}, {1.0f, 1.0f});
        Oven::Renderer2D::DrawQuad({0.0f, 0.0f, -0.1f}, {1.0f, 1.0f}, m_TestSubTexture, {1.0f, 1.0f, 1.0f, 1.0f}, {1.0f, 1.0f});
        Oven::Renderer2D::EndScene(); 

    }
}

void Sandbox2D::OnImGuiRender()
{
    OVEN_PROFILE_FUNCTION();
    auto stats = Oven::Renderer2D::GetStats();
    ImGui::Begin("Renderer2D Stats");
    ImGui::Text("Draw Calls: %d", stats.DrawCalls);
    ImGui::Text("Quads: %d", stats.QuadCount);
    ImGui::Text("Vertices: %d", stats.GetTotalVertexCount());
    ImGui::Text("Indices: %d", stats.GetTotalIndexCount());
    ImGui::Text("Frametime : %f ms, (%d FPS)", Oven::Time::GetDeltaTime() * 1000, (uint32_t)(1.0f/Oven::Time::GetDeltaTime()));
    ImGui::End();
}

void Sandbox2D::OnAttach()
{
    OVEN_PROFILE_FUNCTION();
    m_SandTexture = Oven::Texture2D::Create("sandbox/assets/textures/sand.png");
    m_CheckerboardTexture = Oven::Texture2D::Create("sandbox/assets/textures/checkerboard.png");
    m_SpriteSheet = Oven::Texture2D::Create("sandbox/assets/game/textures/spritesheet_no_padding.png");
    m_TestSubTexture = Oven::SubTexture2D::CreateFromCoords(m_SpriteSheet, {0, 1}, {16, 16}, {3, 3});
}

void Sandbox2D::OnDetach()
{
    OVEN_PROFILE_FUNCTION();
}

void Sandbox2D::OnEvent(Oven::Event &e)
{
m_CameraController.OnEvent(e);
}
