#pragma once
#include "Oven/Core/TimeAPI.h"

namespace Oven
{
class GLFWTimeAPI : public TimeAPI
{
    float GetTime() const override;
};
} // namespace Oven