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
    void OnImGuiRender() override;
    void Begin();
    void End();
    void RescaleContent(float scaleX, float scaleY);
    void ResizeWindow(unsigned int width, unsigned int height);
};
} // namespace Oven