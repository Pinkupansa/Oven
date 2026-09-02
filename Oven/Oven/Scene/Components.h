#pragma once
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "Oven/Scene/SceneCamera.h"
#include "Oven/Scene/NativeScript.h"

namespace Oven
{

struct NameComponent
{
    std::string Name;

    NameComponent() = default;
    NameComponent(const std::string& name) : Name(name) {}
};

struct TransformComponent
{
    glm::vec3 Translation = {0.0f, 0.0f, 0.0f};
    glm::vec3 Rotation = {0.0f, 0.0f, 0.0f};
    glm::vec3 Scale = {1.0f, 1.0f, 1.0f};

    TransformComponent() = default;
    TransformComponent(const glm::vec3& translation) : Translation(translation) {};

    glm::mat4 GetTransform() const
    {
        glm::mat4 rotation = glm::rotate(glm::mat4(1.0f), Rotation.y, {0, 1, 0}) *
                             glm::rotate(glm::mat4(1.0f), Rotation.x, {1, 0, 0}) *
                             glm::rotate(glm::mat4(1.0f), Rotation.z, {0, 0, 1});
        return glm::translate(glm::mat4(1.0f), Translation) * rotation * glm::scale(glm::mat4(1.0f), Scale);
    }
};

struct SpriteRendererComponent
{
    glm::vec4 Color{1.0f, 1.0f, 1.0f, 1.0f};

    SpriteRendererComponent() = default;
    SpriteRendererComponent(const glm::vec4& color) : Color(color) {}
};

enum CameraAspectRatioMode { Adaptive, Fixed };

struct CameraComponent
{
    SceneCamera Camera;
    bool IsMain = true;
    CameraAspectRatioMode AspectRatioMode = Adaptive;

    CameraComponent() = default;
};

struct NativeScriptComponent
{
    NativeScript* Instance = nullptr;

    NativeScript* (*Instantiate)(); // type instanciation on scene play
    void (*DestroyInstance)(NativeScriptComponent*);

    template <typename T> void Bind()
    {
        Instantiate = []() { return static_cast<NativeScript*>(new T()); };
        DestroyInstance = [](NativeScriptComponent* nsc) {
            delete nsc->Instance;
            nsc->Instance = nullptr;
        };
    }
};

} // namespace Oven
