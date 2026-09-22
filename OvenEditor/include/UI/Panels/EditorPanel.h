#pragma once
#include "EditorContext.h"
#include "UI/UIUtils.h"
#include "Oven/Events/Event.h"
namespace Oven
{

class EditorPanel
{

public:
    virtual ~EditorPanel() = default;
    virtual void OnImGuiRender() = 0;
    virtual void OnUpdate() {}
    virtual void OnEvent(Event& e) {}
    virtual void OnSceneChange() {}
    virtual void OnAttach() {}

    void SetContext(EditorContext* context) { m_Context = context; };

    using EventCallbackFn = std::function<void(Event&)>;
    void SetEventCallback(const EventCallbackFn& callback) { m_EventCallback = callback; }

    template <typename T>
        requires std::is_base_of_v<EditorPanel, T>
    static Scope<T> CreatePanel(EditorContext* context)
    {
        auto panel = CreateScope<T>();
        panel->SetContext(context);
        panel->OnAttach();
        return panel;
    }

protected:
    EditorPanel() = default;
    EditorContext* m_Context;
    EventCallbackFn m_EventCallback;
};

} // namespace Oven