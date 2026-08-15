#include "EditorLayer.h"
#include "imgui.h"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <chrono>
#include <iostream>

#include "Oven/Debug/Instrumentor.h"

namespace Oven
{
std::unordered_map<char, Ref<SubTexture2D>> s_TileDict;
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

EditorLayer::EditorLayer() : Layer("OvenEditor"), m_CameraController(1280.0 / 720.0) {}

void EditorLayer::OnUpdate()
{
    OVEN_PROFILE_FUNCTION();
    if (m_SceneTabFocused)
        m_CameraController.OnUpdate();

    Renderer2D::ResetStats();

    RenderCommand::SetClearColor({0.1f, 0.1f, 0.1f, 1});
    RenderCommand::Clear();

    m_Framebuffer->Bind();
    RenderCommand::SetClearColor({0.1f, 0.1f, 0.1f, 1});
    RenderCommand::Clear();

    Renderer2D::BeginScene(m_CameraController.GetCamera());

    m_CurrentScene->OnUpdate();

    Renderer2D::EndScene();
    m_Framebuffer->Unbind();
}

void EditorLayer::OnImGuiRender()
{
    OVEN_PROFILE_FUNCTION();
    ImGui::DockSpaceOverViewport(0, ImGui::GetMainViewport(), ImGuiDockNodeFlags_PassthruCentralNode);
    auto stats = Renderer2D::GetStats();
    ImGui::Begin("Settings");
    ImGui::Text("Draw Calls: %d", stats.DrawCalls);
    ImGui::Text("Quads: %d", stats.QuadCount);
    ImGui::Text("Vertices: %d", stats.GetTotalVertexCount());
    ImGui::Text("Indices: %d", stats.GetTotalIndexCount());
    ImGui::Text("Frametime : %f ms, (%d FPS)", Time::GetDeltaTime() * 1000, (uint32_t)(1.0f / Time::GetDeltaTime()));
    ImGui::End();

    ImGui::Begin("Properties");
    auto& squareColor = m_SquareEntity.GetComponent<SpriteRendererComponent>().Color;
    ImGui::ColorEdit4("Square Color", glm::value_ptr(squareColor));
    ImGui::End();

    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2{0, 0});
    ImGui::Begin("Scene");
    m_SceneTabFocused = ImGui::IsWindowFocused();
    m_SceneTabHovered = ImGui::IsWindowHovered();
    Application::Get().GetImGuiLayer()->SetBlockEvents(!(m_SceneTabFocused && m_SceneTabHovered));

    ImVec2 scenePanelSize = ImGui::GetContentRegionAvail();
    if (m_ScenePanelSize != *((glm::vec2*)&scenePanelSize))
    {
        m_ScenePanelSize = {scenePanelSize.x, scenePanelSize.y};
        m_Framebuffer->Resize((uint32_t)m_ScenePanelSize.x, (uint32_t)m_ScenePanelSize.y);

        m_CameraController.Resize(scenePanelSize.x, scenePanelSize.y);
    }
    uint32_t textureID = m_Framebuffer->GetColorAttachmentRendererID();

    ImGui::Image((void*)textureID, ImVec2{m_ScenePanelSize.x, m_ScenePanelSize.y}, ImVec2(0, 1), ImVec2(1, 0));
    ImGui::End();
    ImGui::PopStyleVar();
}

void EditorLayer::OnAttach()
{
    OVEN_PROFILE_FUNCTION();
    m_SandTexture = Texture2D::Create("OvenEditor/assets/textures/sand.png");
    m_CheckerboardTexture = Texture2D::Create("OvenEditor/assets/textures/checkerboard.png");
    m_SpriteSheet = Texture2D::Create("OvenEditor/assets/game/textures/spritesheet_no_padding.png");
    m_DirtTexture = SubTexture2D::CreateFromCoords(m_SpriteSheet, {6, 31}, {16, 16}, {1, 1});
    m_WaterTexture = SubTexture2D::CreateFromCoords(m_SpriteSheet, {0, 31}, {16, 16}, {1, 1});
    s_TileDict['D'] = m_DirtTexture;
    s_TileDict['W'] = m_WaterTexture;

    m_CameraController.SetZoomLevel(5.0f);
    m_MapWidth = s_MapWidth;
    m_MapHeight = strlen(s_MapTiles) / s_MapWidth;

    FramebufferSpecs fbSpecs;
    fbSpecs.Width = 1280;
    fbSpecs.Height = 720;
    m_Framebuffer = Framebuffer::Create(fbSpecs);

    m_CurrentScene = CreateRef<Scene>();
    m_SquareEntity = m_CurrentScene->CreateEntity("Square");
    m_SquareEntity.AddComponent<SpriteRendererComponent>();
}

void EditorLayer::OnDetach()
{
    OVEN_PROFILE_FUNCTION();
}

void EditorLayer::OnEvent(Event& e)
{
    m_CameraController.OnEvent(e);
}

} // namespace Oven