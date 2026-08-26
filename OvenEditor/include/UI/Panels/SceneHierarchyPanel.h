#pragma once
#include "Oven.h"
#include "UI/Panels/EditorPanel.h"

namespace Oven
{
class SceneHierarchyPanel : public EditorPanel
{
private:
    void DrawEntityNode(Entity entity);

public:
    void OnImGuiRender() override;
};
} // namespace Oven
