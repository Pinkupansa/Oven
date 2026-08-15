#pragma once
#include "Oven.h"
#include <unordered_map>
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
};
} // namespace Oven