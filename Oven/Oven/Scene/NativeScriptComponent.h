#pragma once
#include "Oven/Scene/NativeScript.h"
namespace Oven
{

struct NativeScriptComponent
{
    NativeScript* Instance = nullptr;

    NativeScript* (*Instantiate)(); // type instanciation on scene play
    void (*DestroyInstance)(NativeScriptComponent*);

    template <typename T> void Bind()
    {
        Instantiate = []() { return static_cast<NativeScript*>(new T()); };
        DestroyInstance = [](NativeScriptComponent* nsc) {
            delete nsc->Instance;
            nsc->Instance = nullptr;
        };
    }
};

} // namespace Oven