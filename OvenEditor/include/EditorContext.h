#pragma once
#include "Oven.h"
namespace Oven
{
enum TransformOperation { TRANSLATE = 7, ROTATE = 120, SCALE = 896, NONE = -1 };
enum TransformOperationMode { LOCAL = 0, WORLD };
class EditorContext
{
private:
    Ref<Scene> m_CurrentScene = nullptr;
    Entity m_SelectedEntity = {entt::null, nullptr};
    TransformOperation m_CurrentTransformOperation;
    TransformOperationMode m_CurrentTransformOpMode;
    bool m_IsManipulatingEntity;

public:
    EditorContext() = default;

    EditorContext(const Ref<Scene>& scene) : m_CurrentScene(scene) {}

    Ref<Scene> GetActiveScene() const { return m_CurrentScene; }
    void SetActiveScene(const Ref<Scene>& scene)
    {
        m_CurrentScene = scene;
        ClearSelection();
    }

    void SelectEntity(Entity entity) { m_SelectedEntity = entity; }
    void ClearSelection() { m_SelectedEntity = {entt::null, nullptr}; }
    Entity GetSelectedEntity() const { return m_SelectedEntity; }

    TransformOperation GetCurrentTransformOperation() { return m_CurrentTransformOperation; }
    void SetTransformOperation(TransformOperation op) { m_CurrentTransformOperation = op; }

    TransformOperationMode GetCurrentTransformOperationMode() { return m_CurrentTransformOpMode; }
    void SetTransformOperationMode(TransformOperationMode mode) { m_CurrentTransformOpMode = mode; }

    bool IsManipulatingObject() { return m_IsManipulatingEntity; }
    void SetIsManipulatingEntity(bool isManipulating) { m_IsManipulatingEntity = isManipulating; }
};

} // namespace Oven