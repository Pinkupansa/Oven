#include "EditorLayer.h"
#include "imgui.h"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <chrono>
#include <iostream>

#include "Oven/Debug/Instrumentor.h"

#include "UI/Panels/SceneHierarchyPanel.h"
#include "UI/Panels/PropertiesPanel.h"
#include "UI/Panels/ScenePanel.h"
#include "UI/EditorColors.h"
#include "Oven/Scene/SceneSerializer.h"
#include "Oven/Utils/PlatformUtils.h"

namespace Oven
{

EditorLayer::EditorLayer() : Layer("OvenEditor") {}

static float time = 0;
void EditorLayer::OnUpdate()
{
    OVEN_PROFILE_FUNCTION();

    Renderer2D::ResetStats();

    RenderCommand::SetClearColor({0.1f, 0.1f, 0.1f, 1});
    RenderCommand::Clear();

    for (auto& panel : m_Panels)
    {
        panel->OnUpdate();
    }
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

    for (auto& panel : m_Panels)
    {
        panel->OnImGuiRender();
    }
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

    m_Panels.push_back(EditorPanel::CreatePanel<SceneHierarchyPanel>(&m_Context));
    m_Panels.push_back(EditorPanel::CreatePanel<PropertiesPanel>(&m_Context));
    m_Panels.push_back(EditorPanel::CreatePanel<ScenePanel>(&m_Context));
    std::string sceneFilePath = "OvenEditor/assets/scenes/SuperCube.oven";
    OpenScene(sceneFilePath);
}

void EditorLayer::OnDetach() { OVEN_PROFILE_FUNCTION(); }

void EditorLayer::OnEvent(Event& e)
{

    for (auto& panel : m_Panels)
    {
        panel->OnEvent(e);
    }
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
                if (m_Context.IsManipulatingEntity())
                    return false;
                m_Context.SetTransformOperation(TransformOperation::SCALE);
            }
            break;
        }
        case OvenKey::R: {
            if (m_Context.IsManipulatingEntity())
                return false;
            m_Context.SetTransformOperation(TransformOperation::ROTATE);
            break;
        }
        case OvenKey::T: {
            if (m_Context.IsManipulatingEntity())
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
    OnSceneChange();
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
        OnSceneChange();
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

void EditorLayer::OnSceneChange()
{
    for (auto& panel : m_Panels)
    {
        panel->OnSceneChange();
    }
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