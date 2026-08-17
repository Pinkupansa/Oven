#pragma once
#include "Oven.h"

namespace Oven
{
class SceneHierarchyPanel
{
public:
    SceneHierarchyPanel();
    SceneHierarchyPanel(const Ref<Scene>& scene);
    void SetScene(const Ref<Scene>& scene);
    void OnImGuiRender();

private:
    void AddEntityNode(Entity entity);

private:
    Ref<Scene> m_Scene;
    Entity m_SelectedEntity;
};
} // namespace Oven
