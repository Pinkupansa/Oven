#pragma once
#include "Oven/Debug/Instrumentor.h"
extern Oven::Application* Oven::CreateApplication();

int main(int argc, char** argv)
{
    Oven::Log::Init();
    OVEN_PROFILE_BEGIN_SESSION("Startup", "OvenProfile_Startup.json");
    auto app = Oven::CreateApplication();
    OVEN_PROFILE_END_SESSION();

    OVEN_PROFILE_BEGIN_SESSION("Runtime", "OvenProfile_Runtime.json");
    app->Run();
    OVEN_PROFILE_END_SESSION();

    OVEN_PROFILE_BEGIN_SESSION("Shutdown", "OvenProfile_Shutdown.json");
    delete app;
    OVEN_PROFILE_END_SESSION();
}
