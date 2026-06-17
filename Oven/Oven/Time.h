#pragma once 
#include "Oven/TimeAPI.h"

namespace Oven{
    class Time {
    public:
        static float GetTime();
        static float GetDeltaTime();

    private: 
        friend class Application;
        static void OnUpdate();
    
    private: 
        static TimeAPI* s_timeAPI;
        static float deltaTime;
        static float lastTime;
        static float currentTime;
    };
}