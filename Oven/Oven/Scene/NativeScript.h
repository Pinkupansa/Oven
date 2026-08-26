#pragma once
#include "Oven/Scene/Entity.h"

namespace Oven
{
class NativeScript
{
public:
    template <typename T> T& GetComponent() { return m_Entity.GetComponent<T>(); }

    template <typename T> T& AddComponent() { return m_Entity.AddComponent<T>(); }

    virtual ~NativeScript() {}

protected:
    virtual void OnCreate() {}

    virtual void OnDestroy() {}

    virtual void OnUpdate() {}

private:
    Entity m_Entity;
    friend class Scene;
};
} // namespace Oven
