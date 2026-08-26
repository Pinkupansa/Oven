#include "Oven/ovenpch.h"
#include "Oven/OrthographicCameraController.h"
#include "Oven/Core/Time.h"
#include "Oven/Core/Input.h"
#include "Oven/Core/KeyCodes.h"
#include "Oven/Core/Core.h"

namespace Oven
{

OrthographicCameraController::OrthographicCameraController(float aspectRatio, bool rotation)
    : m_AspectRatio(aspectRatio),
      m_Camera(-m_AspectRatio * m_ZoomLevel, m_AspectRatio * m_ZoomLevel, -m_ZoomLevel, m_ZoomLevel),
      m_RotationEnabled(rotation)
{
}

void OrthographicCameraController::OnUpdate()
{
    OVEN_PROFILE_FUNCTION();

    if (Input::KeyPressed(OvenKey::Right))
    {
        m_CamPos.x += m_CamSpeed * Time::GetDeltaTime();
    }
    if (Input::KeyPressed(OvenKey::Left))
    {
        m_CamPos.x -= m_CamSpeed * Time::GetDeltaTime();
    }
    if (Input::KeyPressed(OvenKey::Up))
    {
        m_CamPos.y += m_CamSpeed * Time::GetDeltaTime();
    }
    if (Input::KeyPressed(OvenKey::Down))
    {
        m_CamPos.y -= m_CamSpeed * Time::GetDeltaTime();
    }
    if (m_RotationEnabled)
    {
        if (Input::KeyPressed(OvenKey::A))
        {
            m_CamRot += m_CamRotSpeed * Time::GetDeltaTime();
        }
        if (Input::KeyPressed(OvenKey::D))
        {
            m_CamRot -= m_CamRotSpeed * Time::GetDeltaTime();
        }
        m_Camera.SetRotation(m_CamRot);
    }
    m_Camera.SetPosition(m_CamPos);
}

void OrthographicCameraController::OnEvent(Event& e)
{
    OVEN_PROFILE_FUNCTION();

    EventDispatcher dispatcher(e);
    dispatcher.Dispatch<MouseScrolledEvent>(OVEN_BIND_EVENT_FN(OrthographicCameraController::OnMouseScrolled));
    dispatcher.Dispatch<WindowResizeEvent>(OVEN_BIND_EVENT_FN(OrthographicCameraController::OnWindowResized));
}

void OrthographicCameraController::SetZoomLevel(float zoomLevel)
{
    m_ZoomLevel = zoomLevel;
    RecalculateView();
}

void OrthographicCameraController::Resize(float width, float height)
{
    m_AspectRatio = width / height;
    RecalculateView();
}

bool OrthographicCameraController::OnMouseScrolled(MouseScrolledEvent& e)
{
    OVEN_PROFILE_FUNCTION();

    SetZoomLevel(std::max(m_ZoomLevel - e.GetYOffset() * 0.25f, 0.25f));

    return false;
}

bool OrthographicCameraController::OnWindowResized(WindowResizeEvent& e)
{
    OVEN_PROFILE_FUNCTION();

    Resize((float)e.GetWidth(), (float)e.GetHeight());
    return false;
}

void OrthographicCameraController::RecalculateView()
{ m_Camera.SetProjectionMatrix(-m_AspectRatio * m_ZoomLevel, m_AspectRatio * m_ZoomLevel, -m_ZoomLevel, m_ZoomLevel); }
} // namespace Oven