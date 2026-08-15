#pragma once

#include "Oven/Core/Core.h"
namespace Oven
{
class OVEN_API Input
{
public:
    static bool KeyPressed(int keycode);
    static bool MouseButtonPressed(int button);
    static float GetMouseX();
    static float GetMouseY();
    static std::pair<float, float> GetMousePosition();
};
} // namespace Oven
