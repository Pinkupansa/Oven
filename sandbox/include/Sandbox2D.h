#pragma once
#include "Oven.h"
#include <unordered_map>
#define N_FRAMES_AVERAGE 60
class Sandbox2D : public Oven::Layer
{
public:
    Sandbox2D();
    virtual ~Sandbox2D() = default;
    void OnUpdate() override;
    void OnImGuiRender() override;
    void OnAttach() override;
    void OnDetach() override;
    void OnEvent(Oven::Event& e) override;

private:
    Oven::OrthographicCameraController m_CameraController;
    Oven::Ref<Oven::Shader> m_FlatColorShader;
    Oven::Ref<Oven::VertexArray> m_SquareVA;
    glm::vec4 m_SquareColor = {0.2, 0.9, 0.1, 1};

    Oven::Ref<Oven::Texture2D> m_SandTexture;
    Oven::Ref<Oven::Texture2D> m_CheckerboardTexture;
    Oven::Ref<Oven::Texture2D> m_SpriteSheet;
    Oven::Ref<Oven::SubTexture2D> m_DirtTexture;
    Oven::Ref<Oven::SubTexture2D> m_WaterTexture;

    uint32_t m_MapWidth, m_MapHeight;
};