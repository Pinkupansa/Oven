#pragma once
#include "Oven/Events/KeyEvent.h"
#include "Oven/Events/MouseEvent.h"
#include "Oven/Events/ApplicationEvent.h"
#include "Oven/Core/Layer.h"

namespace Oven
{
class OVEN_API ImGuiLayer : public Layer
{
public:
    ImGuiLayer();
    ~ImGuiLayer();
    void OnAttach() override;
    void OnDetach() override;
    void OnEvent(Event& e) override;
    void OnImGuiRender() override;
    void Begin();
    void End();
    void RescaleContent(float scaleX, float scaleY);
    void ResizeWindow(uint32_t width, uint32_t height);

    void SetBlockEvents(bool block) { m_BlockEvents = block; }

private:
    bool m_BlockEvents = true;
    bool OnMouseEvent(Event& e);
    bool OnKeyboardEvent(Event& e);
};
} // namespace Oven