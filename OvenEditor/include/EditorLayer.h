#pragma once
#include "Oven.h"
#include <unordered_map>
#include "UI/Panels/EditorPanel.h"
#include "EditorContext.h"
#define N_FRAMES_AVERAGE 60

namespace Oven
{
class EditorLayer : public Layer
{
public:
    EditorLayer();
    virtual ~EditorLayer() = default;
    void OnUpdate() override;
    void OnImGuiRender() override;
    void OnAttach() override;
    void OnDetach() override;
    void OnEvent(Event& e) override;

private:
    void SetDefaultTheme();
    bool OnKeyTyped(KeyPressedEvent& e);

    void NewScene();
    void OpenSceneDialog();
    void OpenScene(std::string& filepath);
    void SaveSceneAsDialog();

private:
    OrthographicCameraController m_CameraController;
    Ref<Shader> m_FlatColorShader;
    Ref<VertexArray> m_SquareVA;
    glm::vec4 m_SquareColor = {0.2, 0.9, 0.1, 1};

    Ref<Texture2D> m_SandTexture;
    Ref<Texture2D> m_CheckerboardTexture;
    Ref<Texture2D> m_SpriteSheet;
    Ref<SubTexture2D> m_DirtTexture;
    Ref<SubTexture2D> m_WaterTexture;

    Ref<Framebuffer> m_Framebuffer;
    glm::vec2 m_ScenePanelSize;
    uint32_t m_MapWidth, m_MapHeight;
    bool m_SceneTabFocused, m_SceneTabHovered = false;

    Entity m_SquareEntity;
    Entity m_CameraEntity;

    // Panels
    std::vector<Scope<EditorPanel>> m_Panels;
    EditorContext m_Context;
};
} // namespace Oven