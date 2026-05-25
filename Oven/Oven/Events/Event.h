#pragma once 

#include "Oven/Core.h"
#include <spdlog/fmt/fmt.h>
namespace Oven{
    //Blocking events, meaning they must be dealt with immediately. Should switch to buffered events

    enum class EventType
    {
        None = 0, 
        WindowClose, WindowResize, WindowFocus, WindowLostFocus, WindowMoved, 
        AppTick, AppUpdate, AppRender,
        KeyPressed, KeyReleased, 
        MouseButtonPressed, MouseButtonReleased, MouseMoved, MouseScrolled
    };


    
    enum EventCategory
    {
        None = 0,
        EventCategoryApplication = BIT(0), //BIT allows to have multiple categories for an event
        EventCategoryInput = BIT(1),
        EventCategoryKeyboard = BIT(2),
        EventCategoryMouse = BIT(3),
        EventCategoryMouseButton = BIT(4),
    };


    //-----------MACROS FOR CUSTOM EVENT DEFINITIONS-----------
    #define EVENT_CLASS_TYPE(type) \
    static EventType GetStaticType() { return EventType::type; } \
    virtual EventType GetEventType() const override { return GetStaticType(); } \
    virtual const char* GetName() const override { return #type; }

#define EVENT_CLASS_CATEGORY(category) \
    virtual int GetCategoryFlags() const override { return category; }
    //---------------------------------------------------------
    class OVEN_API Event{
        friend class EventDispatcher;
        
        public: 
            virtual EventType GetEventType() const = 0;
            virtual const char *GetName() const = 0; 
            virtual int GetCategoryFlags() const = 0;
            virtual std::string ToString() const { return GetName(); } 

            inline bool IsInCategory(EventCategory category){
                return GetCategoryFlags() & category;
            }
        
        protected:
            bool m_Handled = false;
};
class EventDispatcher{
    template<typename T> 
    using EventFn = std::function<bool(T&)>;
    public:
        EventDispatcher(Event& event) : m_Event(event){}
    template <typename T> 
    bool Dispatch(EventFn<T> func){
        if(m_Event.GetEventType() == T::GetStaticType()){
            m_Event.m_Handled = func(*(T*)&m_Event);
            return true;
        }
        return false;
    }      
    private:
        Event& m_Event; 
};


inline std::ostream& operator <<(std::ostream& os, Event& e){
    return os << e.ToString();
}
}
template<typename T>
struct fmt::formatter<T, std::enable_if_t<std::is_base_of_v<Oven::Event, T>, char>>
    : fmt::formatter<std::string>
{
    auto format(const Oven::Event& e, fmt::format_context& ctx) const {
        return fmt::formatter<std::string>::format(e.ToString(), ctx);
    }
};
