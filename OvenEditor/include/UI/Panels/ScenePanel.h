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

private:
    glm::vec2 m_Size = {1.0f, 1.0f};
    bool m_Focused, m_Hovered;
    Ref<Framebuffer> m_Framebuffer;
    EditorCamera m_EditorCamera;
    Entity m_LastSelectedEntity;
};
} // namespace Oven
