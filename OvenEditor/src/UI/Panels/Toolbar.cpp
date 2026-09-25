
#include "UI/Panels/Toolbar.h"
#include "UI/UIUtils.h"
#include <imgui.h>
#include <imgui_internal.h>
#include <cmath>

namespace Oven
{
void Toolbar::OnImGuiRender()
{
    ImGuiWindowClass windowClass;
    windowClass.DockNodeFlagsOverrideSet =
        ImGuiDockNodeFlags_NoTabBar | ImGuiDockNodeFlags_NoWindowMenuButton | ImGuiDockNodeFlags_NoDockingOverMe;
    ImGui::SetNextWindowClass(&windowClass);

    ImGuiWindowFlags flags =
        ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse;

    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f)); // lu par Begin, donc avant
    ImGui::Begin("##Toolbar", nullptr, flags);
    ImGui::PopStyleVar();

    constexpr float margin = 2.0f; // espace au-dessus et en dessous du bouton
    const ImVec2 windowSize = ImGui::GetWindowSize();
    const float buttonSize = std::floor(windowSize.y - margin * 2.0f);
    const float iconSize = std::floor(buttonSize * 0.5f);

    // Centrage horizontal et vertical, arrondi au pixel
    ImGui::SetCursorPos(
        ImVec2(std::floor((windowSize.x - buttonSize) * 0.5f), std::floor((windowSize.y - buttonSize) * 0.5f))
    );

    SceneState sceneState = m_Context->GetSceneState();
    Ref<Texture2D> icon = sceneState == SceneState::Edit ? m_IconPlay : m_IconStop;

    if (UIUtils::TactileImageButton(
            "##PlayStop",
            (ImTextureID)icon->GetRendererID(),
            ImVec2(iconSize, iconSize),    // taille de l'image
            ImVec2(buttonSize, buttonSize) // taille totale du bouton
        ))
    {
        if (sceneState == SceneState::Edit)
            m_Context->SetSceneState(SceneState::Play);
        else if (sceneState == SceneState::Play)
            m_Context->SetSceneState(SceneState::Edit);
    }

    ImGui::End();
}
void Toolbar::OnAttach()
{
    m_IconPlay = Texture2D::Create("OvenEditor/resources/icons/play.png");
    m_IconStop = Texture2D::Create("OvenEditor/resources/icons/stop.png");
}
} // namespace Oven