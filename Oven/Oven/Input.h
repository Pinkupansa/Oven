#pragma once 

#include "Oven/Core.h"
namespace Oven
{
   class OVEN_API Input 
   {
        public:
            inline static bool KeyPressed(int keycode) { return s_Instance->KeyPressedImpl(keycode); }
            inline static bool MouseButtonPressed(int button) {return s_Instance->MouseButtonPressedImpl(button); }
            inline static float GetMouseX(){return s_Instance->GetMouseXImpl(); }
            inline static float GetMouseY(){return s_Instance->GetMouseYImpl(); }
            inline static std::pair<float, float> GetMousePosition() {return s_Instance->GetMousePositionImpl(); }
        protected:
            virtual bool KeyPressedImpl(int keycode) = 0;
            virtual bool MouseButtonPressedImpl(int button) = 0;
            virtual float GetMouseXImpl() = 0;
            virtual float GetMouseYImpl() = 0;
            virtual std::pair<float, float> GetMousePositionImpl() = 0;
        private:
            static Input* s_Instance;
   };
} // namespace Oven
