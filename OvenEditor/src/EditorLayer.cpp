#include "EditorLayer.h"
#include "imgui.h"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <chrono>
#include <iostream>

#include "Oven/Debug/Instrumentor.h"

#include "UI/Panels/SceneHierarchyPanel.h"
#include "UI/Panels/PropertiesPanel.h"

namespace Oven
{

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

    m_Context.GetActiveScene()->OnUpdate();

    m_Framebuffer->Unbind();
}

void EditorLayer::OnImGuiRender()
{
    OVEN_PROFILE_FUNCTION();
    ImGui::DockSpaceOverViewport(0, ImGui::GetMainViewport(), ImGuiDockNodeFlags_PassthruCentralNode);
    auto stats = Renderer2D::GetStats();
    ImGui::Begin("Renderer2D Stats");
    ImGui::Text("Draw Calls: %d", stats.DrawCalls);
    ImGui::Text("Quads: %d", stats.QuadCount);
    ImGui::Text("Vertices: %d", stats.GetTotalVertexCount());
    ImGui::Text("Indices: %d", stats.GetTotalIndexCount());
    ImGui::Text("Frametime : %f ms, (%d FPS)", Time::GetDeltaTime() * 1000, (uint32_t)(1.0f / Time::GetDeltaTime()));
    ImGui::End();

    /*ImGui::Begin("Properties");
    auto& squareColor = m_SquareEntity.GetComponent<SpriteRendererComponent>().Color;
    ImGui::ColorEdit4("Square Color", glm::value_ptr(squareColor));
    auto& camera = m_CameraEntity.GetComponent<CameraComponent>().Camera;
    float orthoSize = camera.GetOrthographicSize();
    if (ImGui::DragFloat("Camera Ortho Size", &orthoSize))
        camera.SetOrthographicSize(orthoSize);
    ImGui::End();*/

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

        m_Context.GetActiveScene()->OnViewportResize((uint32_t)scenePanelSize.x, (uint32_t)scenePanelSize.y);
    }

    uint32_t textureID = m_Framebuffer->GetColorAttachmentRendererID();

    ImGui::Image((void*)textureID, ImVec2{m_ScenePanelSize.x, m_ScenePanelSize.y}, ImVec2(0, 1), ImVec2(1, 0));

    for (auto& panel : m_Panels)
    {
        panel->OnImGuiRender();
    }

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

    m_CameraController.SetZoomLevel(5.0f);

    FramebufferSpecs fbSpecs;
    fbSpecs.Width = 1280;
    fbSpecs.Height = 720;
    m_Framebuffer = Framebuffer::Create(fbSpecs);

    m_Context.SetActiveScene(CreateRef<Scene>());
    m_SquareEntity = m_Context.GetActiveScene()->CreateEntity("Square");
    m_SquareEntity.AddComponent<SpriteRendererComponent>();

    m_CameraEntity = m_Context.GetActiveScene()->CreateEntity("Camera");
    m_CameraEntity.AddComponent<CameraComponent>();

    class CameraController : public NativeScript
    {
    public:
        void OnCreate() {}

        void OnDestroy() {}

        void OnUpdate()
        {
            auto& transform = GetComponent<TransformComponent>().Transform;
            float speed = 5.0f;
            if (Input::KeyPressed(OvenKey::Right))
            {
                transform[3][0] += speed * Time::GetDeltaTime();
            }
            if (Input::KeyPressed(OvenKey::Left))
            {
                transform[3][0] -= speed * Time::GetDeltaTime();
            }
            if (Input::KeyPressed(OvenKey::Up))
            {
                transform[3][1] += speed * Time::GetDeltaTime();
            }
            if (Input::KeyPressed(OvenKey::Down))
            {
                transform[3][1] -= speed * Time::GetDeltaTime();
            }
        }
    };

    m_CameraEntity.AddComponent<NativeScriptComponent>().Bind<CameraController>();
    m_Panels.push_back(EditorPanel::CreatePanel<SceneHierarchyPanel>(&m_Context));
    m_Panels.push_back(EditorPanel::CreatePanel<PropertiesPanel>(&m_Context));
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