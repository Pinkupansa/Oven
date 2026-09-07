#pragma once
#include "Oven.h"
#include "EditorPanel.h"
#include "EditorCamera.h"
#include <imgui.h>
#include <glm/glm.hpp>
namespace Oven
{
class ScenePanel : public EditorPanel
{
public:
    void OnImGuiRender() override;
    void OnUpdate() override;
    void OnEvent(Event& e) override;
    void OnSceneChange() override;
    void OnAttach() override;

private:
    void OnViewportResize();
    void OnNewSelectedEntity();
    bool OnMouseButtonPressed(MouseButtonPressedEvent& e);

private:
    glm::vec2 m_ViewportSize = {1.0f, 1.0f};
    std::array<glm::vec2, 2> m_ViewportScreenSpaceBounds;
    bool m_Focused, m_Hovered;
    Ref<Framebuffer> m_Framebuffer;
    EditorCamera m_EditorCamera;
    Entity m_LastSelectedEntity;
    uint32_t m_ScenePanelID;
    std::string m_PanelName;
    uint32_t m_DrawCount;
    static uint32_t s_ScenePanelCount;
    static uint32_t s_LastFocusedScenePanelID;
};
} // namespace Oven
