#include "WeaponSystem.h"

#include "RendererComponent.h"
#include "WeaponComponent.h"

namespace sand {

//----------------------------------------------------------------------------------------------------
void WeaponSystem::Draw(RendererComponent& rendererComponent, std::weak_ptr<WeaponComponent> weaponComponent, const glm::vec4& color) const
{
    glm::vec3 fromPosition = glm::vec3(weaponComponent.lock()->m_originShot.x, weaponComponent.lock()->m_originShot.y, static_cast<F32>(LayerType::WEAPON));
    glm::vec3 toPosition = glm::vec3(weaponComponent.lock()->m_destinationShot.x, weaponComponent.lock()->m_destinationShot.y, static_cast<F32>(LayerType::WEAPON));
    glLineWidth(1.4f);
    rendererComponent.DrawLine(fromPosition, toPosition, color);
    glLineWidth(1.0f);
}
}
