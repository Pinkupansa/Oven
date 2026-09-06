#include "UI/Panels/ScenePanel.h"
#include "Oven/Renderer/Framebuffer.h"
#include <ImGuizmo.h>
#include <glm/gtc/type_ptr.hpp>
namespace Oven
{

void ScenePanel::OnAttach()
{
    FramebufferSpecs fbSpecs;
    fbSpecs.AttachmentsSpecs = {
        FramebufferTextureFormat::COLOR_UNORM8_RGBA,
        FramebufferTextureFormat::COLOR_INT8_R,
        FramebufferTextureFormat::DEPTH_UNORM24_STENCIL_UINT8
    };
    fbSpecs.Width = 1280;
    fbSpecs.Height = 720;
    m_Framebuffer = Framebuffer::Create(fbSpecs);

    m_Context->SetActiveScene(CreateRef<Scene>());
    m_EditorCamera = EditorCamera(45.0f, 1.778f, 0.1f, 1000.0f, m_Context);
}

void ScenePanel::OnUpdate()
{
    m_Framebuffer->Bind();
    RenderCommand::SetClearColor({0.1f, 0.1f, 0.1f, 1});
    RenderCommand::Clear();

    m_EditorCamera.OnUpdate();
    m_Context->GetActiveScene()->OnUpdateEditor({m_EditorCamera.GetProjection(), m_EditorCamera.GetViewMatrix()});

    auto [mx, my] = ImGui::GetMousePos();
    mx -= m_ViewportScreenSpaceBounds[0].x;
    my -= m_ViewportScreenSpaceBounds[0].y;
    my = m_ViewportSize.y - my;

    int mouseX = (int)mx;
    int mouseY = (int)my;

    if (mx > 0 && my >= 0 && mx <= m_ViewportSize.x && my <= m_ViewportSize.y)
    {
        int pixelData = m_Framebuffer->ReadPixel(1, mouseX, mouseY);
        OVEN_CORE_WARN("Pixel data = {0}", pixelData);
    }

    m_Framebuffer->Unbind();
}
void ScenePanel::OnEvent(Event& e) { m_EditorCamera.OnEvent(e); }

void ScenePanel::OnSceneChange() { OnViewportResize(); }

void ScenePanel::OnViewportResize()
{
    m_Context->GetActiveScene()->OnViewportResize(m_ViewportSize.x, m_ViewportSize.y);
    m_EditorCamera.SetViewportSize(m_ViewportSize.x, m_ViewportSize.y);
}

void ScenePanel::OnNewSelectedEntity()
{
    Entity newSelectedEntity = m_Context->GetSelectedEntity();
    if (newSelectedEntity && newSelectedEntity.HasComponent<TransformComponent>())
    {
        // m_EditorCamera.SetFocalPoint(newSelectedEntity.GetComponent<TransformComponent>().Translation);
    }
}

void ScenePanel::OnImGuiRender()
{
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2{0, 0});
    ImGui::Begin("Scene");
    m_Focused = ImGui::IsWindowFocused();
    m_Hovered = ImGui::IsWindowHovered();

    Application::Get().GetImGuiLayer()->SetBlockEvents(!m_Focused && !m_Hovered);

    ImVec2 newSize = ImGui::GetContentRegionAvail();
    if (m_ViewportSize != *((glm::vec2*)&newSize))
    {
        m_ViewportSize = {newSize.x, newSize.y};
        m_Framebuffer->Resize((uint32_t)m_ViewportSize.x, (uint32_t)m_ViewportSize.y);
        OnViewportResize();
    }

    uint32_t textureID = m_Framebuffer->GetColorAttachmentRendererID(0);

    auto viewportOffset = ImGui::GetCursorPos();
    ImVec2 screenSpaceMin = {ImGui::GetWindowPos().x + viewportOffset.x, ImGui::GetWindowPos().y + viewportOffset.y};
    m_ViewportScreenSpaceBounds[0] = {screenSpaceMin.x, screenSpaceMin.y};
    m_ViewportScreenSpaceBounds[1] = {screenSpaceMin.x + m_ViewportSize.y, screenSpaceMin.y + m_ViewportSize.y};

    ImGui::Image((void*)textureID, ImVec2{m_ViewportSize.x, m_ViewportSize.y}, ImVec2(0, 1), ImVec2(1, 0));

    Entity selectedEntity = m_Context->GetSelectedEntity();
    if (selectedEntity != m_LastSelectedEntity)
    {
        m_LastSelectedEntity = selectedEntity;
        OnNewSelectedEntity();
    }
    m_Context->SetIsManipulatingEntity(ImGuizmo::IsUsing());
    if (selectedEntity && m_Context->GetCurrentTransformOperation() != NONE)
    {
        ImGuizmo::SetOrthographic(false);
        ImGuizmo::SetDrawlist();

        float windowWidth = (float)ImGui::GetWindowWidth();
        float windowHeight = (float)ImGui::GetWindowHeight();
        ImGuizmo::SetRect(ImGui::GetWindowPos().x, ImGui::GetWindowPos().y, windowWidth, windowHeight);

        // Camera
        auto camera = m_Context->GetActiveScene()->GetMainCamera();
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
            if (m_Context->GetCurrentTransformOperation() == TransformOperation::ROTATE)
            {
                snapValue = 45.0f;
            }
            float snapValues[3] = {snapValue, snapValue, snapValue};

            // gizmo manipulation

            ImGuizmo::Manipulate(
                glm::value_ptr(m_EditorCamera.GetViewMatrix()),
                glm::value_ptr(m_EditorCamera.GetProjection()),
                (ImGuizmo::OPERATION)m_Context->GetCurrentTransformOperation(),
                (ImGuizmo::MODE)m_Context->GetCurrentTransformOperationMode(),
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
} // namespace Oven