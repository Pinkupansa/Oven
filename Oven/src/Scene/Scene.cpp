#include "Oven/ovenpch.h"
#include "Oven/Scene/Scene.h"
#include "Oven/Scene/Components.h"
#include "Oven/Scene/Entity.h"
#include "Oven/Renderer/Renderer2D.h"
#include <glm/glm.hpp>

namespace Oven
{
Scene::Scene() {}

Scene::~Scene() {}

Entity Scene::CreateEntity(const std::string& name)
{
    Entity e = {m_Registry.create(), this};
    e.AddComponent<TransformComponent>();
    auto& nameComp = e.AddComponent<NameComponent>();
    nameComp.Name = name.empty() ? "Entity" : name;
    return e;
}

void Scene::OnUpdate()
{
    auto group = m_Registry.group<TransformComponent>(entt::get<SpriteRendererComponent>);
    for (auto entity : group)
    {
        auto [transform, sprite] = group.get<TransformComponent, SpriteRendererComponent>(entity);
        Renderer2D::DrawQuad(transform, sprite.Color);
    }
}
} // namespace Oven