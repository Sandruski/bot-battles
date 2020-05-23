#include "WeaponSystem.h"

#include "RendererComponent.h"
#include "WeaponComponent.h"

namespace sand {

//----------------------------------------------------------------------------------------------------
void WeaponSystem::DebugDraw(RendererComponent& rendererComponent, std::weak_ptr<WeaponComponent> weaponComponent, const glm::vec4& color) const
{
    glm::vec3 fromPosition = glm::vec3(weaponComponent.lock()->m_origin.x, weaponComponent.lock()->m_origin.y, 0.0f);
    glm::vec3 toPosition = glm::vec3(weaponComponent.lock()->m_destination.x, weaponComponent.lock()->m_destination.y, 0.0f);
    rendererComponent.DrawLine(fromPosition, toPosition, color);
}
}
