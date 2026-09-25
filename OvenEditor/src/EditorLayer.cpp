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
#include "UI/Panels/ContentBrowserPanel.h"
#include "UI/Panels/Toolbar.h"
#include "UI/EditorColors.h"
#include "Oven/Scene/SceneSerializer.h"
#include "Oven/Utils/PlatformUtils.h"
#include "misc/freetype/imgui_freetype.h"
#include <filesystem>
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
    Application::Get().GetImGuiLayer()->SetBlockEvents(true);
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
    m_Panels.back()->SetEventCallback(OVEN_BIND_EVENT_FN(EditorLayer::OnEvent));
    // m_Panels.push_back(EditorPanel::CreatePanel<ScenePanel>(&m_Context));
    // m_Panels.push_back(EditorPanel::CreatePanel<ScenePanel>(&m_Context));
    // m_Panels.push_back(EditorPanel::CreatePanel<ScenePanel>(&m_Context));
    m_Panels.push_back(EditorPanel::CreatePanel<ContentBrowserPanel>(&m_Context));
    m_Panels.push_back(EditorPanel::CreatePanel<Toolbar>(&m_Context));
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

    dispatcher.Dispatch<SceneOpenRequestedEvent>(OVEN_BIND_EVENT_FN(EditorLayer::OnSceneOpenRequested));
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

bool EditorLayer::OnSceneOpenRequested(SceneOpenRequestedEvent& e)
{
    OpenScene(e.GetPath());
    return true; // Événement consommé
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

void EditorLayer::OpenScene(const std::filesystem::path& filepath)
{
    if (!filepath.empty())
    {
        m_Context.SetActiveScene(CreateRef<Scene>());
        SceneSerializer serializer(m_Context.GetActiveScene());
        serializer.Deserialize(filepath.string());
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
    io.Fonts->AddFontFromFileTTF("OvenEditor/resources/fonts/Tahoma/static/Tahoma-Bold.ttf", 14.0f);
    io.FontDefault = io.Fonts->AddFontFromFileTTF("OvenEditor/resources/fonts/Tahoma/static/Tahoma-Regular.ttf", 14.0f);
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
    colors[ImGuiCol_Text] = COLOR_TEXT_DEFAULT;
    colors[ImGuiCol_TextDisabled] = COLOR_TEXT_DISABLED;
    colors[ImGuiCol_WindowBg] = COLOR_PANEL_DEFAULT;
    colors[ImGuiCol_ChildBg] = COLOR_PANEL_DEFAULT;
    colors[ImGuiCol_PopupBg] = COLOR_PANEL_DEFAULT;

    // Bordures : laissées par défaut, elles sortaient en gris-bleu.
    colors[ImGuiCol_Border] = COLOR_BORDER_DEFAULT;
    colors[ImGuiCol_BorderShadow] = COLOR_BORDER_SHADOW;

    // Frames
    colors[ImGuiCol_FrameBg] = COLOR_INPUT_FIELD_DEFAULT;
    colors[ImGuiCol_FrameBgHovered] = COLOR_INPUT_FIELD_HOVERED;
    colors[ImGuiCol_FrameBgActive] = COLOR_INPUT_FIELD_HOVERED;

    // Title bars
    // Attention : une fenêtre dockée n'a plus de barre de titre, ces trois slots
    // peignent le fond de la bande d'onglets du nœud de dock. Les garder
    // identiques évite que la sélection colorie la bande au lieu de l'onglet.
    colors[ImGuiCol_TitleBg] = COLOR_MENU_BAR;
    colors[ImGuiCol_TitleBgActive] = COLOR_MENU_BAR;
    colors[ImGuiCol_TitleBgCollapsed] = COLOR_MENU_BAR;

    // Tabs and navigation
    colors[ImGuiCol_Tab] = COLOR_TAB_BG; // Onglet inactif : manquait
    colors[ImGuiCol_TabHovered] = COLOR_HOVERED_DEFAULT;
    colors[ImGuiCol_TabActive] = COLOR_HOVERED_DEFAULT;
    colors[ImGuiCol_TabUnfocused] = COLOR_TAB_TITLE_BG;
    colors[ImGuiCol_TabUnfocusedActive] = COLOR_TAB_TITLE_BG;
    // Les deux traits qui apparaissaient en bleu au-dessus du titre :
    colors[ImGuiCol_TabSelectedOverline] = COLOR_TAB_OVERLINE;              // [1.91+]
    colors[ImGuiCol_TabDimmedSelectedOverline] = COLOR_TAB_OVERLINE_DIMMED; // [1.91+]

    colors[ImGuiCol_TextSelectedBg] = COLOR_TEXT_SELECTED;
    colors[ImGuiCol_NavHighlight] = COLOR_HOVERED_DEFAULT;

    colors[ImGuiCol_Header] = COLOR_TAB_HEADER_BG;
    colors[ImGuiCol_HeaderHovered] = COLOR_HOVERED_DEFAULT;
    colors[ImGuiCol_HeaderActive] = COLOR_FOCUSED_DEFAULT;

    // Separators : manquaient
    colors[ImGuiCol_Separator] = COLOR_SEPARATOR;
    colors[ImGuiCol_SeparatorHovered] = COLOR_SEPARATOR_HOVERED;
    colors[ImGuiCol_SeparatorActive] = COLOR_SEPARATOR_ACTIVE;

    // Buttons
    colors[ImGuiCol_Button] = COLOR_BUTTON_DEFAULT;
    colors[ImGuiCol_ButtonHovered] = COLOR_HOVERED_DEFAULT;
    colors[ImGuiCol_ButtonActive] = COLOR_HOVERED_DEFAULT;

    // Controls
    colors[ImGuiCol_CheckMark] = COLOR_CHECKMARK;
    colors[ImGuiCol_SliderGrab] = COLOR_SLIDER;
    colors[ImGuiCol_SliderGrabActive] = COLOR_FOCUSED_DEFAULT;
    colors[ImGuiCol_TextLink] = COLOR_TEXT_LINK; // [1.91+] manquait

    // Scrollbars
    colors[ImGuiCol_ScrollbarBg] = COLOR_SCROLLBAR_BG;
    colors[ImGuiCol_ScrollbarGrab] = COLOR_SCROLLBAR;
    colors[ImGuiCol_ScrollbarGrabHovered] = COLOR_SCROLLBAR;
    colors[ImGuiCol_ScrollbarGrabActive] = COLOR_SCROLLBAR;

    colors[ImGuiCol_ResizeGrip] = COLOR_RESIZE_GRIP; // Manquait
    colors[ImGuiCol_ResizeGripHovered] = COLOR_SCROLLBAR;
    colors[ImGuiCol_ResizeGripActive] = COLOR_SCROLLBAR;

    // Tables : manquaient
    colors[ImGuiCol_TableHeaderBg] = COLOR_TABLE_HEADER_BG;
    colors[ImGuiCol_TableBorderStrong] = COLOR_TABLE_BORDER_STRONG;
    colors[ImGuiCol_TableBorderLight] = COLOR_TABLE_BORDER_LIGHT;
    colors[ImGuiCol_TableRowBg] = COLOR_TABLE_ROW_BG;
    colors[ImGuiCol_TableRowBgAlt] = COLOR_TABLE_ROW_BG_ALT;

    // Plots : manquaient
    colors[ImGuiCol_PlotLines] = COLOR_PLOT_LINE;
    colors[ImGuiCol_PlotLinesHovered] = COLOR_PLOT_LINE_HOVERED;
    colors[ImGuiCol_PlotHistogram] = COLOR_PLOT_HISTOGRAM;
    colors[ImGuiCol_PlotHistogramHovered] = COLOR_PLOT_HISTOGRAM_HOVERED;

    // Docking : DockingPreview est un bleu vif par défaut
    colors[ImGuiCol_DockingPreview] = COLOR_DOCKING_PREVIEW;
    colors[ImGuiCol_DockingEmptyBg] = COLOR_DOCKING_EMPTY_BG;

    // Misc
    colors[ImGuiCol_NavWindowingHighlight] = COLOR_NAV_WINDOWING_HIGHLIGHT; // Manquait
    colors[ImGuiCol_NavWindowingDimBg] = COLOR_NAV_WINDOWING_DIM_BG;        // Manquait
    colors[ImGuiCol_DragDropTarget] = COLOR_DRAG_DROP_TARGET;               // Manquait
    colors[ImGuiCol_ModalWindowDimBg] = COLOR_MODAL_DIM_BG;
    colors[ImGuiCol_InputTextCursor] = COLOR_TEXT_SELECTED;
    colors[ImGuiCol_CheckboxSelectedBg] = COLOR_INPUT_FIELD_DEFAULT;

    colors[ImGuiCol_MenuBarBg] = COLOR_MENU_BAR;
}

} // namespace Oven