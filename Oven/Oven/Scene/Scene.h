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

    entt::registry& Reg() { return m_Registry; }

private:
    friend class Entity;
    entt::registry m_Registry;
};
} // namespace Oven