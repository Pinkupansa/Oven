#pragma once

#include "entt.hpp"

#include "Oven/Core/Time.h"

namespace Oven
{
class Entity;

class Scene
{
public:
    Scene();
    ~Scene();

    Entity CreateEntity(const std::string& name = "");
    void OnUpdate();
    void OnViewportResize(uint32_t viewportWidth, uint32_t viewportHeight);

    entt::registry& Reg() { return m_Registry; }

private:
    friend class Entity;
    friend class SceneHierarchyPanel;
    entt::registry m_Registry;
    uint32_t m_ViewportWidth, m_ViewportHeight;
};
} // namespace Oven