#pragma once
extern Oven::Application* Oven::CreateApplication();

int main(int argc, char** argv){
    printf("Welcome to the Oven game engine ! \n");
    auto app = Oven::CreateApplication();
    app->Run();
    delete app;
}