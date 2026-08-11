#include "Oven/ovenpch.h"
#include "Oven/Core/Time.h"
#include "Oven/Core/Log.h"

namespace Oven {

    // Initialisation des membres statiques
    TimeAPI* Time::s_timeAPI = TimeAPI::Create();
    float Time::deltaTime = 0.0f;
    float Time::lastTime = -1.0f;
    float Time::currentTime = 0.0f;
    
    float Time::GetTime() {
        return s_timeAPI->GetTime(); 
    }

    float Time::GetDeltaTime() {
        if (lastTime < 0.0f) {
            OVEN_CORE_WARN("Asked for deltaTime on first frame !"); 
            return 1.0f / 60.0f; 
        }
        return deltaTime;
    }

    void Time::OnUpdate() {
        // Si c'est le tout premier update, on synchronise le point de départ
        if (lastTime < 0.0f) {
            currentTime = s_timeAPI->GetTime();
            lastTime = currentTime;
        } else {
            lastTime = currentTime;
            currentTime = s_timeAPI->GetTime();
        }

        deltaTime = currentTime - lastTime;
    }

} // namespace Oven