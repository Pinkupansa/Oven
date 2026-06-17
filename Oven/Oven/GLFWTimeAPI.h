#pragma once 
#include "Oven/TimeAPI.h"

namespace Oven{
    class GLFWTimeAPI : public TimeAPI{
        float GetTime() const override;
    };
}