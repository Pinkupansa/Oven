#pragma once
#include "Oven.h"
#include "EditorPanel.h"

namespace Oven
{
class PropertiesPanel : public EditorPanel
{
public:
    void OnImGuiRender() override;

private:
    void DrawSelectionComponents();
};
} // namespace Oven
