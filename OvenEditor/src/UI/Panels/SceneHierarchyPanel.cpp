#include "UI/Panels/SceneHierarchyPanel.h"
#include "imgui.h"
#include "Oven/Scene/Components.h"

namespace Oven
{

void SceneHierarchyPanel::OnImGuiRender()
{
    ImGui::Begin("Scene Hierarchy");

    m_Context->GetActiveScene()->m_Registry.view<NameComponent>().each(
        [&](auto entity, auto nameComponent) { AddEntityNode({entity, m_Context->GetActiveScene().get()}); }
    );

    if (ImGui::IsMouseDown(0) && ImGui::IsWindowHovered())
    {
        m_Context->ClearSelection();
    }
    ImGui::End();
}

void SceneHierarchyPanel::AddEntityNode(Entity entity)
{

    std::string name = entity.GetComponent<NameComponent>().Name;
    ImGuiTreeNodeFlags flags =
        ImGuiTreeNodeFlags_OpenOnArrow | ((m_Context->GetSelectedEntity() == entity) ? ImGuiTreeNodeFlags_Selected : 0);
    bool opened = ImGui::TreeNodeEx((void*)(uint64_t)(uint32_t)entity, flags, "%s", name.c_str());
    if (ImGui::IsItemClicked())
    {
        m_Context->SelectEntity(entity);
    }
    if (opened)
    {
        ImGui::TreePop();
    }
}

} // namespace Oven