#include "UI/Panels/PropertiesPanel.h"
#include "imgui.h"
#include <cstring> // En C++
#include <glm/gtc/type_ptr.hpp>

namespace Oven
{
void PropertiesPanel::OnImGuiRender()
{
    ImGui::Begin("Properties");
    Entity entity = m_Context->GetSelectedEntity();
    if (entity) //
    {
        if (entity.HasComponent<NameComponent>())
        {
            auto& name = entity.GetComponent<NameComponent>().Name;

            char buffer[256] = {}; // Initialise tous les éléments à 0 (remplace memset)
            snprintf(buffer, sizeof(buffer), "%s", name.c_str());

            if (ImGui::InputText("Name", buffer, sizeof(buffer)))
            {
                name = buffer; // Conversion implicite en std::string
            }
        }
        if (entity.HasComponent<TransformComponent>())
        {
            if (ImGui::TreeNodeEx(
                    (void*)typeid(TransformComponent).hash_code(), ImGuiTreeNodeFlags_DefaultOpen, "Transform"
                ))
            {
                auto& transform = entity.GetComponent<TransformComponent>().Transform;
                ImGui::DragFloat3("Position", glm::value_ptr(transform[3]), 0.1f);
                ImGui::TreePop();
            }
        }
        if (entity.HasComponent<CameraComponent>())
        {
            if (ImGui::TreeNodeEx((void*)typeid(CameraComponent).hash_code(), ImGuiTreeNodeFlags_DefaultOpen, "Camera"))
            {
                auto& cameraComponent = entity.GetComponent<CameraComponent>();
                auto& camera = cameraComponent.Camera;

                // Projection Type
                {
                    const char* projectionTypeStrings[] = {"Perspective", "Orthographic"};
                    const char* currentProjectionTypeString = projectionTypeStrings[(int)camera.GetProjectionType()];
                    if (ImGui::BeginCombo("Projection Mode", currentProjectionTypeString))
                    {

                        for (int i = 0; i < 2; i++)
                        {
                            bool isSelected = currentProjectionTypeString == projectionTypeStrings[i];
                            if (ImGui::Selectable(projectionTypeStrings[i], isSelected))
                            {
                                currentProjectionTypeString = projectionTypeStrings[i];
                                camera.SetProjectionType((SceneCamera::ProjectionType)i);
                            }
                            if (isSelected)
                                ImGui::SetItemDefaultFocus();
                        }

                        ImGui::EndCombo();
                    }
                }

                // Aspect Ratio Mode
                {
                    const char* aspectRatioModeStrings[] = {"Adaptive", "Fixed"};
                    const char* currentAspectRatioModeString =
                        aspectRatioModeStrings[(int)cameraComponent.AspectRatioMode];
                    if (ImGui::BeginCombo("Aspect Ratio Mode", currentAspectRatioModeString))
                    {

                        for (int i = 0; i < 2; i++)
                        {
                            bool isSelected = currentAspectRatioModeString == aspectRatioModeStrings[i];
                            if (ImGui::Selectable(aspectRatioModeStrings[i], isSelected))
                            {
                                currentAspectRatioModeString = aspectRatioModeStrings[i];
                                cameraComponent.AspectRatioMode = (CameraAspectRatioMode)i;
                            }
                            if (isSelected)
                                ImGui::SetItemDefaultFocus();
                        }

                        ImGui::EndCombo();
                    }
                }

                // Perspective Properties
                if (cameraComponent.Camera.GetProjectionType() == SceneCamera::ProjectionType::Perspective)
                {
                    float projFov = glm::degrees(camera.GetPerspectiveVerticalFOV());
                    if (ImGui::DragFloat("Vertical FOV", &projFov))
                        camera.SetPerspectiveVerticalFOV(glm::radians(projFov));

                    float projNear = camera.GetPerspectiveNearClip();
                    if (ImGui::DragFloat("Near Clip", &projNear))
                        camera.SetPerspectiveNearClip(projNear);

                    float projFar = camera.GetPerspectiveFarClip();
                    if (ImGui::DragFloat("Far Clip", &projFar))
                        camera.SetPerspectiveFarClip(projFar);
                }

                // Orthographic Properties
                if (cameraComponent.Camera.GetProjectionType() == SceneCamera::ProjectionType::Orthographic)
                {
                    float orthoSize = camera.GetOrthographicSize();
                    if (ImGui::DragFloat("Size", &orthoSize))
                        camera.SetOrthographicSize(orthoSize);

                    float orthoNear = camera.GetOrthographicNearClip();
                    if (ImGui::DragFloat("Near Clip", &orthoNear))
                        camera.SetOrthographicNearClip(orthoNear);

                    float orthoFar = camera.GetOrthographicFarClip();
                    if (ImGui::DragFloat("Far Clip", &orthoFar))
                        camera.SetOrthographicFarClip(orthoFar);
                }

                ImGui::TreePop();
            }
        }
    }
    ImGui::End();
}

void PropertiesPanel::DrawEntityComponents() {}
} // namespace Oven