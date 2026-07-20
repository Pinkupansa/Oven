#include "Oven/ovenpch.h"
#include "Oven/OrthographicCameraController.h"
#include "Oven/Time.h"
#include "Oven/Input.h"
#include "Oven/KeyCodes.h"
#include "Oven/Core.h"

namespace Oven{

    OrthographicCameraController::OrthographicCameraController(float aspectRatio, bool rotation) 
    :m_AspectRatio(aspectRatio), m_Camera(-m_AspectRatio * m_ZoomLevel, m_AspectRatio*m_ZoomLevel, -m_ZoomLevel, m_ZoomLevel), m_RotationEnabled(rotation){

    }
    void OrthographicCameraController::OnUpdate(){
        if(Input::KeyPressed(OVEN_KEY_RIGHT)){
            m_CamPos.x += m_CamSpeed * Time::GetDeltaTime();
        }
        if(Input::KeyPressed(OVEN_KEY_LEFT)){
            m_CamPos.x -= m_CamSpeed * Time::GetDeltaTime();
        }
        if(Input::KeyPressed(OVEN_KEY_UP)){
            m_CamPos.y += m_CamSpeed * Time::GetDeltaTime();
        }
        if(Input::KeyPressed(OVEN_KEY_DOWN)){
            m_CamPos.y -= m_CamSpeed * Time::GetDeltaTime();
        }
        if(m_RotationEnabled){
            if(Input::KeyPressed(OVEN_KEY_A)){
                m_CamRot += m_CamRotSpeed * Time::GetDeltaTime();
            }
            if(Input::KeyPressed(OVEN_KEY_D)){
                m_CamRot -= m_CamRotSpeed * Time::GetDeltaTime();
            }
            m_Camera.SetRotation(m_CamRot);
        }
        m_Camera.SetPosition(m_CamPos);
    }

    void OrthographicCameraController::OnEvent(Event& e){
        EventDispatcher dispatcher(e); 
        dispatcher.Dispatch<MouseScrolledEvent>(OVEN_BIND_EVENT_FN(OrthographicCameraController::OnMouseScrolled));
        dispatcher.Dispatch<WindowResizeEvent>(OVEN_BIND_EVENT_FN(OrthographicCameraController::OnWindowResized));
    }

    bool OrthographicCameraController::OnMouseScrolled(MouseScrolledEvent& e){

        m_ZoomLevel -= e.GetYOffset() * 0.25;
        m_ZoomLevel = std::max(m_ZoomLevel, 0.25f);
        m_Camera.SetProjection( -m_AspectRatio * m_ZoomLevel, m_AspectRatio*m_ZoomLevel, -m_ZoomLevel, m_ZoomLevel); 
        return false;
    }
    bool OrthographicCameraController::OnWindowResized(WindowResizeEvent& e){
        m_AspectRatio = (float)e.GetWidth() / (float)e.GetHeight();
        m_Camera.SetProjection( -m_AspectRatio * m_ZoomLevel, m_AspectRatio*m_ZoomLevel, -m_ZoomLevel, m_ZoomLevel);
        return false;
    }
}