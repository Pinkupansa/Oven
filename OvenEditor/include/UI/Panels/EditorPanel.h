#pragma once
#include "EditorContext.h"

namespace Oven
{
class EditorPanel
{
protected:
    EditorPanel() = default;
    EditorContext* m_Context;

public:
    virtual ~EditorPanel() = default;
    virtual void OnImGuiRender() = 0;

    void SetContext(EditorContext* context) { m_Context = context; };

    template <typename T>
        requires std::is_base_of_v<EditorPanel, T>
    static Scope<T> CreatePanel(EditorContext* context)
    {
        auto panel = CreateScope<T>();
        panel->SetContext(context);
        return panel;
    }
};

} // namespace Oven