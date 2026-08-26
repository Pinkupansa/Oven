#include "EditorLayer.h"
#include "imgui.h"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <chrono>
#include <iostream>

#include "Oven/Debug/Instrumentor.h"

#include "UI/Panels/SceneHierarchyPanel.h"
#include "UI/Panels/PropertiesPanel.h"
#include "UI/EditorColors.h"
#include "Oven/Scene/SceneSerializer.h"

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
    ImGuiStyle& style = ImGui::GetStyle();
    float minWinSizeX = style.WindowMinSize.x;
    style.WindowMinSize.x = 200.0f;
    ImGui::DockSpaceOverViewport(0, ImGui::GetMainViewport(), ImGuiDockNodeFlags_PassthruCentralNode);
    style.WindowMinSize.x = minWinSizeX;
    auto stats = Renderer2D::GetStats();
    ImGui::Begin("Renderer2D Stats");
    UIUtils::PanelContentSeparator();
    ImGui::Text("Draw Calls: %d", stats.DrawCalls);
    ImGui::Text("Quads: %d", stats.QuadCount);
    ImGui::Text("Vertices: %d", stats.GetTotalVertexCount());
    ImGui::Text("Indices: %d", stats.GetTotalIndexCount());
    ImGui::Text("Frametime : %f ms, (%d FPS)", Time::GetDeltaTime() * 1000, (uint32_t)(1.0f / Time::GetDeltaTime()));
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

    SetDefaultTheme();

    m_SandTexture = Texture2D::Create("OvenEditor/assets/textures/sand.png");
    m_CheckerboardTexture = Texture2D::Create("OvenEditor/assets/textures/checkerboard.png");
    m_SpriteSheet = Texture2D::Create("OvenEditor/assets/game/textures/spritesheet_no_padding.png");
    m_DirtTexture = SubTexture2D::CreateFromCoords(m_SpriteSheet, {6, 31}, {16, 16}, {1, 1});
    m_WaterTexture = SubTexture2D::CreateFromCoords(m_SpriteSheet, {0, 31}, {16, 16}, {1, 1});

    FramebufferSpecs fbSpecs;
    fbSpecs.Width = 1280;
    fbSpecs.Height = 720;
    m_Framebuffer = Framebuffer::Create(fbSpecs);

    m_Context.SetActiveScene(CreateRef<Scene>());
#if 0
    m_CameraController.SetZoomLevel(5.0f);

    m_SquareEntity = m_Context.GetActiveScene()->CreateEntity("Square");
    m_SquareEntity.AddComponent<SpriteRendererComponent>();

    m_CameraEntity = m_Context.GetActiveScene()->CreateEntity("Camera");
    m_CameraEntity.AddComponent<CameraComponent>();

    class CameraController : public NativeScript
    {
    public:
        ~CameraController() {}
        void OnCreate() {}

        void OnDestroy() {}

        void OnUpdate()
        {
            auto& translation = GetComponent<TransformComponent>().Translation;
            float speed = 5.0f;
            if (Input::KeyPressed(OvenKey::Right))
            {
                translation.x += speed * Time::GetDeltaTime();
            }
            if (Input::KeyPressed(OvenKey::Left))
            {
                translation.x -= speed * Time::GetDeltaTime();
            }
            if (Input::KeyPressed(OvenKey::Up))
            {
                translation.y += speed * Time::GetDeltaTime();
            }
            if (Input::KeyPressed(OvenKey::Down))
            {
                translation.y -= speed * Time::GetDeltaTime();
            }
        }
    };

    m_CameraEntity.AddComponent<NativeScriptComponent>().Bind<CameraController>();
#endif

    m_Panels.push_back(EditorPanel::CreatePanel<SceneHierarchyPanel>(&m_Context));
    m_Panels.push_back(EditorPanel::CreatePanel<PropertiesPanel>(&m_Context));

    SceneSerializer serializer(m_Context.GetActiveScene());
    serializer.Deserialize("OvenEditor/assets/scenes/Example.oven");
}

void EditorLayer::OnDetach() { OVEN_PROFILE_FUNCTION(); }

void EditorLayer::OnEvent(Event& e) { m_CameraController.OnEvent(e); }

void EditorLayer::SetDefaultTheme()
{
    ImGuiIO& io = ImGui::GetIO();
    io.Fonts->AddFontFromFileTTF("OvenEditor/assets/fonts/Tahoma/static/Tahoma-Bold.ttf", 14.0f);
    io.FontDefault = io.Fonts->AddFontFromFileTTF("OvenEditor/assets/fonts/Tahoma/static/Tahoma-Regular.ttf", 14.0f);
    io.Fonts->Build();
    ImGuiStyle& style = ImGui::GetStyle();
    ImVec4* colors = style.Colors;

    // Geometry
    style.WindowRounding = 2.0f;
    style.ChildRounding = 2.0f;
    style.FrameRounding = 2.0f;
    style.PopupRounding = 2.0f;
    style.ScrollbarRounding = 2.0f;
    style.GrabRounding = 2.0f;
    style.TabRounding = 2.0f;

    style.WindowBorderSize = 1.0f;
    style.ChildBorderSize = 1.0f;
    style.PopupBorderSize = 1.0f;
    style.FrameBorderSize = 1.0f;  // clear input-field definition
    style.TabBorderSize = 0.0f;    // no ring around individual tabs — the bar border below does the separating job
    style.TabBarBorderSize = 1.0f; // border along the bottom of the tab strip itself

    style.FramePadding = ImVec2(6.0f, 4.0f);
    style.WindowPadding = ImVec2(8.0f, 8.0f);
    style.ItemSpacing = ImVec2(6.0f, 6.0f);
    style.DockingSeparatorSize = 1.0f;

    // Misc
    style.WindowMenuButtonPosition = ImGuiDir_None;

    io.MouseDrawCursor = false; // Let OS / GLFW / SDL handle the cursor rendering
    io.ConfigDockingAlwaysTabBar = true;

    // ============================================================
    // TEXT & CANVAS
    // ============================================================
    colors[ImGuiCol_Text] = COLOR_CHARCOAL_DARK;
    colors[ImGuiCol_TextDisabled] = COLOR_STEEL_GRAY;
    colors[ImGuiCol_WindowBg] = COLOR_COOL_WHITE;
    colors[ImGuiCol_ChildBg] = COLOR_COOL_WHITE;
    colors[ImGuiCol_PopupBg] = COLOR_COOL_WHITE;

    // Border darkened a touch below SLATE_TRIM's raw value so it actually
    // reads against PORCELAIN_WHITE/COOL_WHITE instead of blending into it.
    colors[ImGuiCol_Border] = COLOR_SLATE_TRIM;
    colors[ImGuiCol_BorderShadow] = ImVec4(0.000f, 0.000f, 0.000f, 0.00f);

    // Frames
    colors[ImGuiCol_FrameBg] = COLOR_PORCELAIN_WHITE;
    colors[ImGuiCol_FrameBgHovered] = COLOR_COOL_WHITE;
    colors[ImGuiCol_FrameBgActive] = COLOR_COOL_WHITE;

    // Title bars
    colors[ImGuiCol_TitleBg] = COLOR_STEEL_GRAY;
    colors[ImGuiCol_TitleBgActive] = COLOR_STEEL_GRAY;
    colors[ImGuiCol_TitleBgCollapsed] = COLOR_STEEL_GRAY;

    // Tabs and navigation
    colors[ImGuiCol_Tab] = COLOR_PORCELAIN_WHITE;
    colors[ImGuiCol_TabHovered] = COLOR_ACCENT_ORANGE_LIGHT;
    colors[ImGuiCol_TabActive] = COLOR_ACCENT_ORANGE_LIGHT;
    colors[ImGuiCol_TabUnfocused] = COLOR_PORCELAIN_WHITE;
    colors[ImGuiCol_TabUnfocusedActive] = COLOR_PORCELAIN_WHITE;
    colors[ImGuiCol_TextSelectedBg] = COLOR_HOVER_CYAN;
    colors[ImGuiCol_NavHighlight] = COLOR_ACCENT_ORANGE_LIGHT;

    colors[ImGuiCol_Header] = COLOR_COOL_WHITE;
    colors[ImGuiCol_HeaderHovered] = COLOR_ACCENT_ORANGE_LIGHT;
    colors[ImGuiCol_HeaderActive] = COLOR_ACCENT_ORANGE;

    // Buttons
    colors[ImGuiCol_Button] = COLOR_COOL_WHITE;
    colors[ImGuiCol_ButtonHovered] = COLOR_ACCENT_ORANGE_LIGHT;
    colors[ImGuiCol_ButtonActive] = COLOR_ACCENT_ORANGE_LIGHT;

    // Controls
    colors[ImGuiCol_CheckMark] = COLOR_ACCENT_ORANGE;
    colors[ImGuiCol_SliderGrab] = COLOR_SLATE_MUTED;
    colors[ImGuiCol_SliderGrabActive] = COLOR_ACCENT_ORANGE;

    // Scrollbars
    colors[ImGuiCol_ScrollbarBg] = COLOR_PORCELAIN_WHITE;
    colors[ImGuiCol_ScrollbarGrab] = COLOR_STEEL_GRAY;
    colors[ImGuiCol_ScrollbarGrabHovered] = COLOR_INDICATOR_CYAN;
    colors[ImGuiCol_ScrollbarGrabActive] = COLOR_ACCENT_ORANGE;

    colors[ImGuiCol_ResizeGripHovered] = COLOR_STEEL_GRAY;
    colors[ImGuiCol_ResizeGripActive] = COLOR_ACCENT_ORANGE;

    // Plots
    colors[ImGuiCol_PlotLines] = COLOR_MUTED_GREEN;
    colors[ImGuiCol_PlotLinesHovered] = COLOR_ALERT_RED;
    colors[ImGuiCol_PlotHistogram] = COLOR_SLATE_MUTED;
    colors[ImGuiCol_PlotHistogramHovered] = COLOR_ACCENT_ORANGE;

    // Misc
    colors[ImGuiCol_ModalWindowDimBg] = ImVec4(0.000f, 0.000f, 0.000f, 0.35f);
    colors[ImGuiCol_InputTextCursor] = COLOR_INDICATOR_CYAN;
    colors[ImGuiCol_CheckboxSelectedBg] = COLOR_PORCELAIN_WHITE;
}

} // namespace Oven