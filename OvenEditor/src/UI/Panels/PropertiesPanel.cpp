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
    }
    ImGui::End();
}

void PropertiesPanel::DrawEntityComponents() {}
} // namespace Oven