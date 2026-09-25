#pragma once
#include "Oven.h"
namespace Oven
{
enum class SceneState { Edit = 0, Play = 1 };
enum TransformOperation { TRANSLATE = 7, ROTATE = 120, SCALE = 896, NONE = -1 };
enum TransformOperationMode { LOCAL = 0, WORLD };
struct EditorContext
{
private:
    Ref<Scene> m_CurrentScene = nullptr;
    Entity m_SelectedEntity = {entt::null, nullptr};
    TransformOperation m_CurrentTransformOperation = TransformOperation::TRANSLATE;
    TransformOperationMode m_CurrentTransformOpMode;
    bool m_IsManipulatingEntity;
    SceneState m_SceneState;

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

    SceneState GetSceneState() { return m_SceneState; }
    void SetSceneState(SceneState state) { m_SceneState = state; }

    bool IsManipulatingEntity() { return m_IsManipulatingEntity; }
    void SetIsManipulatingEntity(bool isManipulating) { m_IsManipulatingEntity = isManipulating; }
};

} // namespace Oven