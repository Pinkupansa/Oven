#include "Oven/ovenpch.h"
#include "Oven/GLFWTimeAPI.h"
#include <GLFW/glfw3.h>
namespace Oven{
    float GLFWTimeAPI::GetTime() const{
        return glfwGetTime();
    }
}