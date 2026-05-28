#include <iostream> 
#include <Oven.h>
class Sandbox : public Oven::Application
{
    public:
        Sandbox()
        {
            PushOverlay(new Oven::ImGuiLayer());
        }
        ~Sandbox()
        {

        }
};

Oven::Application* Oven::CreateApplication(){
    printf("Launching Sandbox App ...\n");
    return new Sandbox();
}