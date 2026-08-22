#pragma once
#include "Oven.h"

namespace Oven
{
class EditorContext
{
private:
    Ref<Scene> m_CurrentScene = nullptr;
    Entity m_SelectedEntity = {entt::null, nullptr};

public:
    EditorContext() = default;

    EditorContext(const Ref<Scene>& scene) : m_CurrentScene(scene) {}

    void SetActiveScene(const Ref<Scene>& scene)
    {
        m_CurrentScene = scene;
        ClearSelection();
    }

    void SelectEntity(Entity entity) { m_SelectedEntity = entity; }

    void ClearSelection() { m_SelectedEntity = {entt::null, nullptr}; }

    Entity GetSelectedEntity() const { return m_SelectedEntity; }

    Ref<Scene> GetActiveScene() const { return m_CurrentScene; }
};

} // namespace Oven