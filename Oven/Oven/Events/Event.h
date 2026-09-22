#pragma once

#include "Oven/Core/Core.h"
#include <spdlog/fmt/fmt.h>
#include <functional>
#include <string>

namespace Oven
{

// Enum class à sous-type fixe uint32_t :
// 1. Compatible avec les forward declarations (enum class EventType;)
// 2. Extensible à l'infini grâce au hash FNV-1a
enum class EventType : uint32_t {};

enum EventCategory {
    None = 0,
    EventCategoryApplication = BIT(0),
    EventCategoryInput = BIT(1),
    EventCategoryKeyboard = BIT(2),
    EventCategoryMouse = BIT(3),
    EventCategoryMouseButton = BIT(4),
};

// Fonction de hachage FNV-1a à la compilation
constexpr uint32_t HashEventType(const char* str)
{
    uint32_t hash = 2166136261u;
    while (*str)
    {
        hash ^= static_cast<uint32_t>(*str++);
        hash *= 16777619u;
    }
    return hash;
}

//-----------MACROS FOR CUSTOM EVENT DEFINITIONS-----------
#define EVENT_CLASS_TYPE(type)                                                                                         \
    static EventType GetStaticType() { return static_cast<EventType>(::Oven::HashEventType(#type)); }                  \
    virtual EventType GetEventType() const override { return GetStaticType(); }                                        \
    virtual const char* GetName() const override { return #type; }

#define EVENT_CLASS_CATEGORY(category)                                                                                 \
    virtual int GetCategoryFlags() const override { return category; }
//---------------------------------------------------------

class OVEN_API Event
{
    friend class EventDispatcher;

public:
    virtual ~Event() = default;

    virtual EventType GetEventType() const = 0;
    virtual const char* GetName() const = 0;
    virtual int GetCategoryFlags() const = 0;
    virtual std::string ToString() const { return GetName(); }

    inline bool IsInCategory(EventCategory category) { return GetCategoryFlags() & category; }
    inline bool Handled() const { return m_Handled; }

protected:
    bool m_Handled = false;
};

class EventDispatcher
{
    template <typename T> using EventFn = std::function<bool(T&)>;

public:
    EventDispatcher(Event& event) : m_Event(event) {}

    template <typename T> bool Dispatch(EventFn<T> func)
    {
        if (m_Event.GetEventType() == T::GetStaticType())
        {
            m_Event.m_Handled = func(*(T*)&m_Event);
            return true;
        }
        return false;
    }

    bool DispatchCategory(EventFn<Event> func, EventCategory category)
    {
        if (m_Event.IsInCategory(category))
        {
            m_Event.m_Handled = func(m_Event);
            return true;
        }
        return false;
    }

private:
    Event& m_Event;
};

inline std::ostream& operator<<(std::ostream& os, Event& e) { return os << e.ToString(); }

} // namespace Oven

template <typename T>
struct fmt::formatter<T, std::enable_if_t<std::is_base_of_v<Oven::Event, T>, char>> : fmt::formatter<std::string>
{
    auto format(const Oven::Event& e, fmt::format_context& ctx) const
    { return fmt::formatter<std::string>::format(e.ToString(), ctx); }
};