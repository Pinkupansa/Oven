#include "Sandbox2D.h"
#include "imgui.h"
#include "Oven/Core/Time.h"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <chrono>
#include <iostream>

#include "Oven/Debug/Instrumentor.h"

std::unordered_map<char, Oven::Ref<Oven::SubTexture2D>> s_TileDict;
static uint32_t s_MapWidth = 23;
static const char* s_MapTiles = "WWWWWWWWWWWWWWWWWWWWWWW"
                                "WWWWWWWWDDDDDDWWWWWWWWW"
                                "WWWWWWDDDDDDDDDDDWWWWWW"
                                "WWWDDDDDDWWWWDDDDDWwWWW"
                                "WWWDDDDDWWWWWWDDDDDWWWW"
                                "WWWDDDDDDWWWWDDDDDDWWWW"
                                "WWWWWWDDDDDDDDDDDWWWWWW"
                                "WWWWWWWWDDDDDDDWWWWWWWW"
                                "WWWWWWWWWWWWWWWWWWWWWWW";

Sandbox2D::Sandbox2D() : Layer("Sandbox2D"), m_CameraController(1280.0 / 720.0) {}

void Sandbox2D::OnUpdate()
{
    OVEN_PROFILE_FUNCTION();
    m_CameraController.OnUpdate();
    Oven::Renderer2D::ResetStats();
    // Render
    {
        OVEN_PROFILE_SCOPE("Renderer Preparation");

        Oven::RenderCommand::SetClearColor({0.1f, 0.1f, 0.1f, 1});
        Oven::RenderCommand::Clear();
    }

    {
        OVEN_PROFILE_SCOPE("Renderer Draw");
        Oven::Renderer2D::BeginScene(m_CameraController.GetCamera());

        for (uint32_t y = 0; y < m_MapHeight; y++)
        {
            for (uint32_t x = 0; x < m_MapWidth; x++)
            {
                char tileType = s_MapTiles[x + y * m_MapWidth];
                Oven::Ref<Oven::SubTexture2D> texture;
                if (s_TileDict.find(tileType) != s_TileDict.end())
                    texture = s_TileDict[tileType];
                else
                    texture = m_DirtTexture;
                Oven::Renderer2D::DrawQuad(
                    {x - m_MapWidth / 2.0f, y - m_MapHeight / 2.0f, 0.5f}, {1.0f, 1.0f}, texture);
            }
        }
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
    ImGui::Text("Frametime : %f ms, (%d FPS)",
                Oven::Time::GetDeltaTime() * 1000,
                (uint32_t)(1.0f / Oven::Time::GetDeltaTime()));

    auto pos = m_CameraController.GetCamera().GetPosition();
    ImGui::Text("Camera Pos: %.2f, %.2f, %.2f", pos.x, pos.y, pos.z);
    ImGui::End();
}

void Sandbox2D::OnAttach()
{
    OVEN_PROFILE_FUNCTION();
    m_SandTexture = Oven::Texture2D::Create("sandbox/assets/textures/sand.png");
    m_CheckerboardTexture = Oven::Texture2D::Create("sandbox/assets/textures/checkerboard.png");
    m_SpriteSheet = Oven::Texture2D::Create("sandbox/assets/game/textures/spritesheet_no_padding.png");
    m_DirtTexture = Oven::SubTexture2D::CreateFromCoords(m_SpriteSheet, {6, 31}, {16, 16}, {1, 1});
    m_WaterTexture = Oven::SubTexture2D::CreateFromCoords(m_SpriteSheet, {0, 31}, {16, 16}, {1, 1});
    s_TileDict['D'] = m_DirtTexture;
    s_TileDict['W'] = m_WaterTexture;

    m_CameraController.SetZoomLevel(5.0f);
    m_MapWidth = s_MapWidth;
    m_MapHeight = strlen(s_MapTiles) / s_MapWidth;

    Oven::FramebufferSpecs fbSpecs;
    fbSpecs.Width = 1280;
    fbSpecs.Height = 720;
}

void Sandbox2D::OnDetach()
{
    OVEN_PROFILE_FUNCTION();
}

void Sandbox2D::OnEvent(Oven::Event& e)
{
    m_CameraController.OnEvent(e);
}
