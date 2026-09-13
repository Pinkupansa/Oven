#pragma once
#include <glm/glm.hpp>
namespace Oven
{
struct SpriteRendererComponent
{
    glm::vec4 Color{1.0f, 1.0f, 1.0f, 1.0f};

    SpriteRendererComponent() = default;
    SpriteRendererComponent(const glm::vec4& color) : Color(color) {}
};
} // namespace Oven