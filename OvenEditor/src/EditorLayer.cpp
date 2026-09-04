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
#include "Oven/Utils/PlatformUtils.h"
#include "ImGuizmo.h"
namespace Oven
{

EditorLayer::EditorLayer() : Layer("OvenEditor"), m_CameraController(1280.0 / 720.0), m_ScenePanelSize(900, 1200) {}

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

    m_EditorCamera.OnUpdate();
    m_Context.GetActiveScene()->OnUpdateEditor({m_EditorCamera.GetProjection(), m_EditorCamera.GetViewMatrix()});

    m_Framebuffer->Unbind();
}

void EditorLayer::OnImGuiRender()
{
    OVEN_PROFILE_FUNCTION();
    ImGuiStyle& style = ImGui::GetStyle();
    float minWinSizeX = style.WindowMinSize.x;
    style.WindowMinSize.x = 200.0f;
    ImGui::DockSpaceOverViewport(0, ImGui::GetMainViewport(), ImGuiDockNodeFlags_PassthruCentralNode);
    // 2. Créer la barre de menu principale
    if (ImGui::BeginMainMenuBar())
    {
        if (ImGui::BeginMenu("File"))
        {
            if (ImGui::MenuItem("New", "Ctrl+N"))
                NewScene();
            if (ImGui::MenuItem("Open...", "Ctrl+O"))
                OpenSceneDialog();
            if (ImGui::MenuItem("Save As...", "Ctrl+Shift+S"))
                SaveSceneAsDialog();

            ImGui::EndMenu();
        }
        ImGui::Separator();
    }
    ImGui::EndMainMenuBar();

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
        OnViewportResize();
    }

    uint32_t textureID = m_Framebuffer->GetColorAttachmentRendererID();

    ImGui::Image((void*)textureID, ImVec2{m_ScenePanelSize.x, m_ScenePanelSize.y}, ImVec2(0, 1), ImVec2(1, 0));

    for (auto& panel : m_Panels)
    {
        panel->OnImGuiRender();
    }
    // Gizmos
    Entity selectedEntity = m_Context.GetSelectedEntity();

    m_Context.SetIsManipulatingEntity(ImGuizmo::IsUsing());
    if (selectedEntity && m_Context.GetCurrentTransformOperation() != NONE)
    {
        ImGuizmo::SetOrthographic(false);
        ImGuizmo::SetDrawlist();

        float windowWidth = (float)ImGui::GetWindowWidth();
        float windowHeight = (float)ImGui::GetWindowHeight();
        ImGuizmo::SetRect(ImGui::GetWindowPos().x, ImGui::GetWindowPos().y, windowWidth, windowHeight);

        // Camera
        auto camera = m_Context.GetActiveScene()->GetMainCamera();
        if (camera)
        {
            const glm::mat4 cameraProjection = camera.GetComponent<CameraComponent>().Camera.GetProjection();
            glm::mat4 cameraView = glm::inverse(camera.GetComponent<TransformComponent>().GetTransform());

            // Entity transform
            auto& transformComponent = selectedEntity.GetComponent<TransformComponent>();
            glm::mat4 transform = transformComponent.GetTransform();

            // snapping
            bool snap = Input::KeyPressed(OvenKey::LeftShift);

            float snapValue = 0.5f;
            if (m_Context.GetCurrentTransformOperation() == TransformOperation::ROTATE)
            {
                snapValue = 45.0f;
            }
            float snapValues[3] = {snapValue, snapValue, snapValue};

            // gizmo manipulation

            ImGuizmo::Manipulate(
                glm::value_ptr(m_EditorCamera.GetViewMatrix()),
                glm::value_ptr(m_EditorCamera.GetProjection()),
                (ImGuizmo::OPERATION)m_Context.GetCurrentTransformOperation(),
                (ImGuizmo::MODE)m_Context.GetCurrentTransformOperationMode(),
                glm::value_ptr(transform),
                nullptr,
                snap ? snapValues : nullptr
            );

            if (ImGuizmo::IsUsing())
            {
                glm::vec3 translation, rotation, scale;
                Maths::DecomposeTransform(transform, translation, rotation, scale);
                transformComponent.Translation = translation;
                transformComponent.Rotation = rotation;
                transformComponent.Scale = scale;
            }
        }
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
    m_EditorCamera = EditorCamera(45.0f, 1.778f, 0.1f, 1000.0f, &m_Context);
    m_Panels.push_back(EditorPanel::CreatePanel<SceneHierarchyPanel>(&m_Context));
    m_Panels.push_back(EditorPanel::CreatePanel<PropertiesPanel>(&m_Context));
    std::string sceneFilePath = "OvenEditor/assets/scenes/SuperCube.oven";
    OpenScene(sceneFilePath);
}

void EditorLayer::OnDetach() { OVEN_PROFILE_FUNCTION(); }

void EditorLayer::OnEvent(Event& e)
{

    m_EditorCamera.OnEvent(e);
    m_CameraController.OnEvent(e);
    EventDispatcher dispatcher(e);
    dispatcher.Dispatch<KeyPressedEvent>(OVEN_BIND_EVENT_FN(EditorLayer::OnKeyTyped));
}

bool EditorLayer::OnKeyTyped(KeyPressedEvent& e)
{
    // Shortcuts
    if (e.GetRepeatCount() > 0)
        return false;

    bool controlPressed = Input::KeyPressed(OvenKey::LeftControl) || Input::KeyPressed(OvenKey::RightControl);
    bool shiftPressed = Input::KeyPressed(OvenKey::LeftShift) || Input::KeyPressed(OvenKey::RightShift);
    switch (e.GetKeyCode())
    {
        case OvenKey::N: {
            if (controlPressed)
                NewScene();
            break;
        }
        case OvenKey::O: {
            if (controlPressed)
                OpenSceneDialog();
            break;
        }
        case OvenKey::S: {
            if (controlPressed and shiftPressed)
            {
                SaveSceneAsDialog();
            }
            else
            {
                if (ImGuizmo::IsUsing())
                    return false;
                m_Context.SetTransformOperation(TransformOperation::SCALE);
            }
            break;
        }
        case OvenKey::R: {
            if (ImGuizmo::IsUsing())
                return false;
            m_Context.SetTransformOperation(TransformOperation::ROTATE);
            break;
        }
        case OvenKey::T: {
            if (ImGuizmo::IsUsing())
                return false;
            m_Context.SetTransformOperation(TransformOperation::TRANSLATE);
            break;
        }

        default:
            break;
    }
    return true;
}

void EditorLayer::NewScene()
{
    m_Context.SetActiveScene(CreateRef<Scene>());
    OnViewportResize();
}

void EditorLayer::OpenSceneDialog()
{
    std::string filepath = FileDialogs::OpenFile("Oven Scene (*.oven)\0*.oven\0");

    OpenScene(filepath);
}

void EditorLayer::OpenScene(std::string& filepath)
{
    if (!filepath.empty())
    {
        m_Context.SetActiveScene(CreateRef<Scene>());
        SceneSerializer serializer(m_Context.GetActiveScene());
        serializer.Deserialize(filepath);
        OnViewportResize();
    }
}

void EditorLayer::SaveSceneAsDialog()
{
    std::string filepath = FileDialogs::SaveFile("Oven Scene (*.oven)\0*.oven\0");
    if (!filepath.empty())
    {
        SceneSerializer serializer(m_Context.GetActiveScene());
        serializer.Serialize(filepath);
    }
}

void EditorLayer::OnViewportResize()
{
    m_Context.GetActiveScene()->OnViewportResize(m_ScenePanelSize.x, m_ScenePanelSize.y);
    m_EditorCamera.SetViewportSize(m_ScenePanelSize.x, m_ScenePanelSize.y);
}

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
    style.FrameRounding = 0.0f;
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

    colors[ImGuiCol_MenuBarBg] = COLOR_COOL_WHITE;
}

} // namespace Oven