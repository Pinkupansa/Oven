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

void Scene::DestroyEntity(Entity entity) { m_Registry.destroy(entity); }

void Scene::OnUpdate()
{
    {
        // Run scripts
        m_Registry.view<NativeScriptComponent>().each([=](auto entity, auto& nsc) {
            if (!nsc.Instance)
            {
                nsc.Instance = nsc.Instantiate();
                nsc.Instance->m_Entity = {entity, this};
                nsc.Instance->OnCreate();
            }
            nsc.Instance->OnUpdate();
        });
    }
    Camera* mainCamera = nullptr;
    glm::mat4 mainCamTransform;
    {
        // Change group to view
        auto view = m_Registry.view<TransformComponent, CameraComponent>();

        for (auto camEntity : view)
        {
            auto [transform, camera] = view.get<TransformComponent, CameraComponent>(camEntity);
            if (camera.IsMain)
            {
                mainCamera = &camera.Camera;
                mainCamTransform = transform.GetTransform();
                break;
            }
        }
    }

    if (mainCamera)
    {

        Renderer2D::BeginScene({mainCamera->GetProjection(), mainCamTransform});
        // Change group to view
        auto group = m_Registry.group<TransformComponent, SpriteRendererComponent>();
        for (auto entity : group)
        {

            auto [transform, sprite] = group.get<TransformComponent, SpriteRendererComponent>(entity);
            Renderer2D::DrawQuad(transform.GetTransform(), sprite.Color);
        }
        Renderer2D::EndScene();
    }
}

void Scene::OnViewportResize(uint32_t viewportWidth, uint32_t viewportHeight)
{
    m_ViewportWidth = viewportWidth;
    m_ViewportHeight = viewportHeight;

    auto view = m_Registry.view<CameraComponent>();

    for (auto entity : view)
    {
        auto& cameraComponent = view.get<CameraComponent>(entity);
        if (cameraComponent.AspectRatioMode == CameraAspectRatioMode::Adaptive)
        {
            cameraComponent.Camera.SetViewportSize(viewportWidth, viewportHeight);
        }
    }
}
template <typename T> inline void Scene::OnComponentAdded(Entity entity) { static_assert(false); }

template <> void Scene::OnComponentAdded<TransformComponent>(Entity entity) {}
template <> void Scene::OnComponentAdded<CameraComponent>(Entity entity)
{ entity.GetComponent<CameraComponent>().Camera.SetViewportSize(m_ViewportWidth, m_ViewportHeight); }
template <> void Scene::OnComponentAdded<SpriteRendererComponent>(Entity entity) {}
template <> void Scene::OnComponentAdded<NameComponent>(Entity entity) {}

template <> void Scene::OnComponentAdded<NativeScriptComponent>(Entity entity) {}
} // namespace Oven