#include "UI/Panels/SceneHierarchyPanel.h"
#include "imgui.h"
#include "Oven/Scene/Components.h"

namespace Oven
{

SceneHierarchyPanel::SceneHierarchyPanel(const Ref<Scene>& scene)
{
    SetScene(scene);
}

void SceneHierarchyPanel::SetScene(const Ref<Scene>& scene)
{
    m_Scene = scene;
}

void SceneHierarchyPanel::OnImGuiRender()
{
    ImGui::Begin("Scene Hierarchy");

    m_Scene->m_Registry.view<NameComponent>().each([&](auto entity, auto nameComponent)
                                                   { AddEntityNode({entity, m_Scene.get()}); });
    ImGui::End();
}

void SceneHierarchyPanel::AddEntityNode(Entity entity)
{
    std::string name = entity.GetComponent<NameComponent>().Name;
    ImGuiTreeNodeFlags flags =
        ImGuiTreeNodeFlags_OpenOnArrow | ((m_SelectedEntity == entity) ? ImGuiTreeNodeFlags_Selected : 0);
    bool opened = ImGui::TreeNodeEx((void*)(uint64_t)(uint32_t)entity, flags, name.c_str());
    if (ImGui::IsItemClicked())
    {
        m_SelectedEntity = entity;
    }
    if (opened)
    {
        ImGui::TreePop();
    }
}

} // namespace Oven