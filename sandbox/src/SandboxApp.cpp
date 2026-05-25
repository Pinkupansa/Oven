#include <iostream> 
#include <Oven.h>

class Sandbox : public Oven::Application
{
    public:
        Sandbox()
        {

        }
        ~Sandbox()
        {

        }
};

Oven::Application* Oven::CreateApplication(){
    printf("Launching Sandbox App ...\n");
    return new Sandbox();
}