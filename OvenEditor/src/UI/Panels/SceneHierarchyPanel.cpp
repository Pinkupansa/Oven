#include "UI/Panels/SceneHierarchyPanel.h"
#include "imgui.h"
#include "Oven/Scene/Components.h"

namespace Oven
{

void SceneHierarchyPanel::OnImGuiRender()
{
    ImGui::Begin("Scene Entities");
    UIUtils::PanelContentSeparator();
    m_Context->GetActiveScene()->m_Registry.view<NameComponent>().each([&](auto entity, auto nameComponent) {
        DrawEntityNode({entity, m_Context->GetActiveScene().get()});
    });

    UIUtils::CenterElement(100);

    if (UIUtils::TactileGradientButton("Create Entity", {100.0f, 20.0f}))
    {

        Entity e = m_Context->GetActiveScene()->CreateEntity("Entity");
        m_Context->SelectEntity(e);
    }
    if (ImGui::IsMouseDown(0) && ImGui::IsWindowHovered())
        m_Context->ClearSelection();

    if (ImGui::BeginPopupContextWindow(
            "##HierarchyContext", ImGuiPopupFlags_MouseButtonRight | ImGuiPopupFlags_NoOpenOverItems
        ))
    {
        if (UIUtils::MenuItem("Create Entity"))
        {
            Entity e = m_Context->GetActiveScene()->CreateEntity("Entity");
            m_Context->SelectEntity(e);
        }
        ImGui::EndPopup();
    }
    ImGui::End();
}

void SceneHierarchyPanel::DrawEntityNode(Entity entity)
{

    std::string name = entity.GetComponent<NameComponent>().Name;
    ImGuiTreeNodeFlags flags =
        ImGuiTreeNodeFlags_OpenOnArrow | ((m_Context->GetSelectedEntity() == entity) ? ImGuiTreeNodeFlags_Selected : 0);
    flags |= ImGuiTreeNodeFlags_SpanAvailWidth;
    // Since TreeNodeEx uses header color we need to push it temporarily to orange
    ImGui::PushStyleColor(ImGuiCol_Header, COLOR_ACCENT_ORANGE_LIGHT);
    bool opened = ImGui::TreeNodeEx((void*)(uint64_t)(uint32_t)entity, flags, "%s", name.c_str());
    ImGui::PopStyleColor();
    bool entityDeleted = false;

    if (ImGui::IsItemClicked())
    {
        m_Context->SelectEntity(entity);
    }
    if (ImGui::BeginPopupContextItem())
    {
        if (UIUtils::MenuItem("Delete Entity"))
            entityDeleted = true;
        ImGui::EndPopup();
    }
    if (opened)
    {
        ImGui::TreePop();
    }
    if (entityDeleted)
    {

        m_Context->GetActiveScene()->DestroyEntity(entity);
        if (m_Context->GetSelectedEntity() == entity)
        {
            m_Context->ClearSelection();
        }
    }
}

} // namespace Oven