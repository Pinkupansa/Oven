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
    glm::mat4* mainCamTransform = nullptr;
    {
        // Change group to view
        auto view = m_Registry.view<TransformComponent, CameraComponent>();

        for (auto camEntity : view)
        {
            auto [transform, camera] = view.get<TransformComponent, CameraComponent>(camEntity);
            if (camera.IsMain)
            {
                mainCamera = &camera.Camera;
                mainCamTransform = &transform.Transform;
                break;
            }
        }
    }

    if (mainCamera)
    {

        Renderer2D::BeginScene({mainCamera->GetProjection(), *mainCamTransform});
        // Change group to view
        auto group = m_Registry.group<TransformComponent, SpriteRendererComponent>();
        for (auto entity : group)
        {
            auto [transform, sprite] = group.get<TransformComponent, SpriteRendererComponent>(entity);
            Renderer2D::DrawQuad(transform, sprite.Color);
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
} // namespace Oven