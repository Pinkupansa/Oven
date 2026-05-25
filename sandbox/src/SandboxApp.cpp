#include <iostream> 
#include <Oven.h>

class ExampleLayer : public Oven::Layer{
    public:
        ExampleLayer()
            : Layer("Example") {}

        void OnUpdate() override{
            OVEN_INFO("ExampleLayer::Update");
        }
        void OnEvent(Oven::Event& event) override{
            OVEN_TRACE("{0}", event);
        }
};
class Sandbox : public Oven::Application
{
    public:
        Sandbox()
        {
            PushLayer(new ExampleLayer());
        }
        ~Sandbox()
        {

        }
};

Oven::Application* Oven::CreateApplication(){
    printf("Launching Sandbox App ...\n");
    return new Sandbox();
}