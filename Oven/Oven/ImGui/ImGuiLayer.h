#pragma once 
#include "Oven/Events/KeyEvent.h"
#include "Oven/Events/MouseEvent.h"
#include "Oven/Events/ApplicationEvent.h"
#include "Oven/Layer.h"
namespace Oven{
    class OVEN_API ImGuiLayer : public Layer{
        public:
            ImGuiLayer();
            ~ImGuiLayer();
            void OnAttach();
            void OnDetach();
            void OnUpdate();
            void OnEvent(Event& event);
            
            void ResizeWindow(unsigned int width, unsigned int height);
            void RescaleContent(float scaleX, float scaleY);
        private:
            bool OnMouseButtonPressedEvent(MouseButtonPressedEvent& e);
            bool OnMouseButtonReleasedEvent(MouseButtonReleasedEvent& e);
            bool OnMouseMovedEvent(MouseMovedEvent& e);
            bool OnMouseScrolledEvent(MouseScrolledEvent& e);
            bool OnKeyPressedEvent(KeyPressedEvent& e);
            bool OnKeyTypedEvent(KeyTypedEvent& e);
            bool OnKeyReleasedEvent(KeyReleasedEvent& e);
            bool OnWindowResizeEvent(WindowResizeEvent& e);
            bool OnWindowContentScaleEvent(WindowContentScaleEvent& e);
        private:
            float m_Time = 0.0f;
    };
}