#include "Oven/TimeAPI.h"
#include "Oven/Log.h"
#include "Oven/Renderer/Renderer.h"
#include "Oven/GLFWTimeAPI.h"

namespace Oven{
    TimeAPI::TimingBackend s_Backend = TimeAPI::TimingBackend::GLFW;

    TimeAPI* TimeAPI::Create(){
        //should this depend on the renderer or the OS? 
        switch(s_Backend){
            case (TimeAPI::TimingBackend::None) : OVEN_CORE_ASSERT(false, "TimingBackend::None is not supported !"); return nullptr; 
            case (TimeAPI::TimingBackend::GLFW) : return new GLFWTimeAPI();
        }        
    }   
}