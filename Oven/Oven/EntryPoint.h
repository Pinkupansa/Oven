#pragma once
extern Oven::Application* Oven::CreateApplication();

int main(int argc, char** argv){
    Oven::Log::Init();

    OVEN_CORE_WARN("Initialized Log !");
    OVEN_CORE_INFO("Welcome to the Oven game engine !");
    OVEN_INFO("Hello !");
    auto app = Oven::CreateApplication();
    app->Run();
    delete app;
}
