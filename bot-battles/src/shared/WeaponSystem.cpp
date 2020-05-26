#include "WeaponSystem.h"

#include "RendererComponent.h"
#include "WeaponComponent.h"

namespace sand {

//----------------------------------------------------------------------------------------------------
void WeaponSystem::Draw(RendererComponent& rendererComponent, std::weak_ptr<WeaponComponent> weaponComponent, const glm::vec4& color) const
{
    glm::vec3 fromPosition = glm::vec3(weaponComponent.lock()->m_origin.x, weaponComponent.lock()->m_origin.y, static_cast<F32>(LayerType::WEAPON));
    glm::vec3 toPosition = glm::vec3(weaponComponent.lock()->m_destination.x, weaponComponent.lock()->m_destination.y, static_cast<F32>(LayerType::WEAPON));
    rendererComponent.DrawLine(fromPosition, toPosition, color);
}
}
