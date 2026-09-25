#pragma once

#include <cstdint>
#include <functional>

namespace Oven
{

class UUID
{
public:
    UUID();
    UUID(uint64_t uuid);
    UUID(const UUID&) = default;

    operator uint64_t() const { return m_UUID; }

private:
    uint64_t m_UUID;
};

} // namespace Oven

namespace std
{

template <> struct hash<Oven::UUID>
{
    std::size_t operator()(const Oven::UUID& uuid) const { return hash<uint64_t>()((uint64_t)uuid); }
};

} // namespace std