#include "Oven/ovenpch.h"
#include "Oven/Scene/SceneCamera.h"
#include <glm/gtc/matrix_transform.hpp>

namespace Oven
{
SceneCamera::SceneCamera() {}

void SceneCamera::SetOrthographicProperties(float size, float nearClip, float farClip)
{
    m_OrthographicSize = size;
    m_OrthographicFar = farClip;
    m_OrthographicNear = nearClip;
    RecalculateProjection();
}

void SceneCamera::SetViewportSize(uint32_t width, uint32_t height)
{
    m_AspectRatio = (float)width / (float)height;
    RecalculateProjection();
}

void SceneCamera::RecalculateProjection()
{

    float orthoLeft = -m_OrthographicSize * m_AspectRatio * 0.5f;
    float orthoRight = m_OrthographicSize * m_AspectRatio * 0.5f;
    float orthoBottom = -m_OrthographicSize * 0.5f;
    float orthoTop = m_OrthographicSize * 0.5f;

    m_Projection = glm::ortho(orthoLeft, orthoRight, orthoBottom, orthoTop, m_OrthographicNear, m_OrthographicFar);
}
} // namespace Oven