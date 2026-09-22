#pragma once

#include "Oven/Events/Event.h"
#include <filesystem>

namespace Oven
{

class SceneOpenRequestedEvent : public Event
{
public:
    SceneOpenRequestedEvent(const std::filesystem::path& path) : m_Path(path) {}

    const std::filesystem::path& GetPath() const { return m_Path; }

    EVENT_CLASS_TYPE(SceneOpenRequestedEvent)
    EVENT_CLASS_CATEGORY(None)

private:
    std::filesystem::path m_Path;
};

} // namespace Oven