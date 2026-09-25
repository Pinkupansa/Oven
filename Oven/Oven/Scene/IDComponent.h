#pragma once
#include "Oven/Core/UUID.h"

namespace Oven
{
struct IDComponent
{
    UUID ID;
    IDComponent() = default;
    IDComponent(const UUID& id) : ID(id) {};
};
} // namespace Oven
// namespace Oven
