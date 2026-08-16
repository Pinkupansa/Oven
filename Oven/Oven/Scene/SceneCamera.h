#pragma once

#include "Oven/Renderer/Camera.h"

namespace Oven
{
class SceneCamera : public Camera
{
public:
    SceneCamera();
    void SetOrthographicProperties(float size, float nearClip, float farClip);

    void SetOrthographicSize(float size)
    {
        m_OrthographicSize = size;
        RecalculateProjection();
    }

    float GetOrthographicSize() const { return m_OrthographicSize; }

    void SetViewportSize(uint32_t width, uint32_t height);

private:
    void RecalculateProjection();

private:
    float m_OrthographicSize = 10.0f;
    float m_OrthographicNear = -1.0f, m_OrthographicFar = 1.0f;
    float m_AspectRatio = 16.0f / 9.0f;
};
} // namespace Oven
