#pragma once 

namespace Oven
{
    class TimeAPI{
        public:
            enum class TimingBackend{
                None = 0, GLFW = 1
            };
        
        public:
            static TimingBackend GetTimingBackend() { return s_TimingBackend; }
        private: 
            friend class Time;
            
            static TimingBackend s_TimingBackend; 
            static TimeAPI* Create(); 

        private:
        virtual float GetTime() const = 0;
    };
} // namespace Oven

