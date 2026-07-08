#pragma once 


#include "Oven/Renderer/OrthographicCamera.h"
#include "Oven/Events/ApplicationEvent.h"
#include "Oven/Events/MouseEvent.h"

namespace Oven{ 
    class OrthographicCameraController{
        public:
        OrthographicCameraController(float aspectRatio, bool rotation = false); 
            void OnUpdate();
            void OnEvent(Event& e);

            OrthographicCamera& GetCamera() {return m_Camera;}
            const OrthographicCamera& GetCamera() const {return m_Camera;}
        private:
            bool OnMouseScrolled(MouseScrolledEvent& e);
            bool OnWindowResized(WindowResizeEvent& e);
        private:
            float m_AspectRatio;
            float m_ZoomLevel = 1.0f;
            OrthographicCamera m_Camera;
            
            bool m_RotationEnabled;
            glm::vec3 m_CamPos = {0, 0, 0}; 
            float m_CamRot = 0;
            float m_CamSpeed = 4.0f, m_CamRotSpeed = 180.0f;
    };
}