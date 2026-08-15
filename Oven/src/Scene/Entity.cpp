#include "Oven/ovenpch.h"
#include "Oven/Scene/Entity.h"

namespace Oven
{

Entity::Entity(entt::entity handle, Scene* scene) : m_EntityHandle(handle), m_Scene(scene) {}

} // namespace Oven