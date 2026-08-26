#include "UI/Panels/PropertiesPanel.h"
#include "imgui.h"
#include <cstring> // En C++
#include <glm/gtc/type_ptr.hpp>
#include "UI/UIUtils.h"

namespace Oven
{
void PropertiesPanel::OnImGuiRender()
{
    ImGui::Begin("Properties");
    UIUtils::PanelContentSeparator();
    Entity entity = m_Context->GetSelectedEntity();
    if (entity)
    {
        DrawSelectionComponents();
        UIUtils::CenterElement(100);
        if (UIUtils::TactileGradientButton("Add Component", {100, 20}))
            ImGui::OpenPopup("AddComponent");

        if (ImGui::BeginPopup("AddComponent"))
        {
            if (UIUtils::MenuItem("Camera"))
            {
                entity.AddComponent<CameraComponent>();
                ImGui::CloseCurrentPopup();
            }
            if (UIUtils::MenuItem("Sprite Renderer"))
            {
                entity.AddComponent<SpriteRendererComponent>();
                ImGui::CloseCurrentPopup();
            }
            ImGui::EndPopup();
        }
    }

    ImGui::End();
}

void PropertiesPanel::DrawSelectionComponents()
{
    Entity entity = m_Context->GetSelectedEntity();
    if (entity) //
    {

        if (entity.HasComponent<NameComponent>())
        {
            ImGui::SetCursorPosX(ImGui::GetCursorPosX() + 24.0f);
            ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2{4, 4});
            auto& name = entity.GetComponent<NameComponent>().Name;

            char buffer[256] = {}; // Initialise tous les éléments à 0 (remplace memset)
            snprintf(buffer, sizeof(buffer), "%s", name.c_str());

            if (UIUtils::InputField("Name", buffer))
            {
                name = buffer; // Conversion implicit into std::string
            }
            ImGui::PopStyleVar();
        }

        UIUtils::DrawComponent<TransformComponent>("Transform", entity, [](auto& transformComponent) {
            UIUtils::DrawVec3Control("Translation", transformComponent.Translation);

            glm::vec3 rotation = glm::degrees(transformComponent.Rotation);
            UIUtils::DrawVec3Control("Rotation", rotation);
            transformComponent.Rotation = glm::radians(rotation);
            UIUtils::DrawVec3Control("Scale", transformComponent.Scale, 1.0f);
        });
        UIUtils::DrawComponent<CameraComponent>("Camera", entity, [](auto& cameraComponent) {
            auto& camera = cameraComponent.Camera;

            // IsMain
            UIUtils::Checkbox("Is Main", cameraComponent.IsMain);

            // Projection Type
            {
                const char* projectionTypeStrings[] = {"Perspective", "Orthographic"};
                SceneCamera::ProjectionType projType = camera.GetProjectionType();
                UIUtils::DrawComboControl("Projection", projType, projectionTypeStrings, 2);
                camera.SetProjectionType(projType);
            }

            // Aspect Ratio Mode
            {
                const char* aspectRatioModeStrings[] = {"Adaptive", "Fixed"};
                UIUtils::DrawComboControl("Aspect Ratio", cameraComponent.AspectRatioMode, aspectRatioModeStrings, 2);
            }

            // Perspective Properties
            if (cameraComponent.Camera.GetProjectionType() == SceneCamera::ProjectionType::Perspective)
            {
                float projFov = glm::degrees(camera.GetPerspectiveVerticalFOV());
                if (UIUtils::DrawFloatControl("##VerticalFOV", projFov))
                    camera.SetPerspectiveVerticalFOV(glm::radians(projFov));

                float projNear = camera.GetPerspectiveNearClip();
                if (UIUtils::DrawFloatControl("##NearClip", projNear))
                    camera.SetPerspectiveNearClip(projNear);

                float projFar = camera.GetPerspectiveFarClip();
                if (UIUtils::DrawFloatControl("##FarClip", projFar))
                    camera.SetPerspectiveFarClip(projFar);
            }

            // Orthographic Properties
            if (cameraComponent.Camera.GetProjectionType() == SceneCamera::ProjectionType::Orthographic)
            {
                float orthoSize = camera.GetOrthographicSize();
                if (UIUtils::DrawFloatControl("Size", orthoSize, 1))
                    camera.SetOrthographicSize(orthoSize);

                float orthoNear = camera.GetOrthographicNearClip();
                if (UIUtils::DrawFloatControl("Near Clip", orthoNear, 0.1f))
                    camera.SetOrthographicNearClip(orthoNear);

                float orthoFar = camera.GetOrthographicFarClip();
                if (UIUtils::DrawFloatControl("Far Clip", orthoFar))
                    camera.SetOrthographicFarClip(orthoFar);
            }
        });

        UIUtils::DrawComponent<SpriteRendererComponent>("Sprite Renderer", entity, [](auto& component) {
            ImGui::ColorEdit4("Color", glm::value_ptr(component.Color));
        });
    }
}
} // namespace Oven