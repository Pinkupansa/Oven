#pragma once

#include "entt.hpp"

#include "Oven/Core/Time.h"
#include "Oven/Renderer/Renderer2D.h"
namespace Oven
{
class Entity;

class Scene
{
public:
    Scene();
    ~Scene();

    Entity CreateEntity(const std::string& name = "");
    void DestroyEntity(Entity entity);
    void OnUpdateRuntime();
    void OnUpdateEditor(Renderer2D::CameraRenderData camera);
    void OnViewportResize(uint32_t viewportWidth, uint32_t viewportHeight);
    Entity GetMainCamera();
    entt::registry& Reg() { return m_Registry; }

private:
    template <typename T> void OnComponentAdded(Entity entity);

private:
    friend class Entity;
    friend class SceneHierarchyPanel;
    friend class SceneSerializer;

    entt::registry m_Registry;
    uint32_t m_ViewportWidth, m_ViewportHeight;
};

} // namespace Oven