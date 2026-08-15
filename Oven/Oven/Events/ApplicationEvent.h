#pragma once

#include "Oven/Events/Event.h"
#include <sstream>
namespace Oven
{
class OVEN_API WindowResizeEvent : public Event
{
public:
    WindowResizeEvent(uint32_t width, uint32_t height) : m_Width(width), m_Height(height) {}

    inline uint32_t GetWidth() const { return m_Width; }
    inline uint32_t GetHeight() const { return m_Height; }

    std::string ToString() const override
    {
        std::stringstream ss;
        ss << "WindowResizeEvent: " << m_Width << ", " << m_Height;
        return ss.str();
    }

    EVENT_CLASS_TYPE(WindowResize)
    EVENT_CLASS_CATEGORY(EventCategoryApplication)

private:
    uint32_t m_Width, m_Height;
};

class OVEN_API WindowContentScaleEvent : public Event
{
public:
    WindowContentScaleEvent(float xScale, float yScale) : m_XScale(xScale), m_YScale(yScale) {}

    inline float GetXScale() const { return m_XScale; }
    inline float GetYScale() const { return m_YScale; }

    std::string ToString() const override
    {
        std::stringstream ss;
        ss << "WindowContentScaleEvent: " << m_XScale << ", " << m_YScale;
        return ss.str();
    }

    EVENT_CLASS_TYPE(WindowContentScale)
    EVENT_CLASS_CATEGORY(EventCategoryApplication)

private:
    float m_XScale, m_YScale;
};

class OVEN_API WindowCloseEvent : public Event
{
public:
    WindowCloseEvent() {}

    EVENT_CLASS_TYPE(WindowClose)
    EVENT_CLASS_CATEGORY(EventCategoryApplication)
};

class OVEN_API AppTickEvent : public Event
{
public:
    AppTickEvent() {}

    EVENT_CLASS_TYPE(AppTick)
    EVENT_CLASS_CATEGORY(EventCategoryApplication)
};

class OVEN_API AppUpdateEvent : public Event
{
public:
    AppUpdateEvent() {}

    EVENT_CLASS_TYPE(AppUpdate)
    EVENT_CLASS_CATEGORY(EventCategoryApplication)
};

class OVEN_API AppRenderEvent : public Event
{
public:
    AppRenderEvent() {}

    EVENT_CLASS_TYPE(AppRender)
    EVENT_CLASS_CATEGORY(EventCategoryApplication)
};
} // namespace Oven