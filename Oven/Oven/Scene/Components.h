#pragma once
#include <glm/glm.hpp>
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
    glm::mat4 Transform{1.0f};

    TransformComponent() = default;
    TransformComponent(const glm::mat4& transform) : Transform(transform) {};

    operator glm::mat4() { return Transform; }
};

struct SpriteRendererComponent
{
    glm::vec4 Color{1.0f, 1.0f, 1.0f, 1.0f};
    SpriteRendererComponent() = default;

    SpriteRendererComponent(const glm::vec4& color) : Color(color) {}
};

enum CameraAspectRatioMode
{
    Adaptive,
    Fixed
};

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
        DestroyInstance = [](NativeScriptComponent* nsc)
        {
            delete nsc->Instance;
            nsc->Instance = nullptr;
        };
    }
};

} // namespace Oven
