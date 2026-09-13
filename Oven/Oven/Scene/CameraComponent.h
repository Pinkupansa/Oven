#pragma once
#include "Oven/Scene/SceneCamera.h"
namespace Oven
{

enum CameraAspectRatioMode { Adaptive, Fixed };

struct CameraComponent
{
    SceneCamera Camera;
    bool IsMain = true;
    CameraAspectRatioMode AspectRatioMode = Adaptive;

    CameraComponent() = default;
};
} // namespace Oven