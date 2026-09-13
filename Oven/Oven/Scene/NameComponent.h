#pragma once
namespace Oven
{
struct NameComponent
{
    std::string Name;

    NameComponent() = default;
    NameComponent(const std::string& name) : Name(name) {}
};
} // namespace Oven