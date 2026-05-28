#pragma once 

#include "Oven/Input.h"

namespace Oven{
    class OVEN_API MacInput : public Input{
        protected:
            bool KeyPressedImpl(int keycode) override;
            bool MouseButtonPressedImpl(int button) override;
            float GetMouseXImpl() override; 
            float GetMouseYImpl() override;
            std::pair<float, float> GetMousePositionImpl() override;
};
}