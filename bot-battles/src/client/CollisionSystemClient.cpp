#include "CollisionSystemClient.h"

#include "ColliderComponent.h"
#include "ComponentManager.h"
#include "ComponentMemberTypes.h"
#include "GameClient.h"
#include "RendererComponent.h"
#include "TransformComponent.h"

namespace sand {

//----------------------------------------------------------------------------------------------------
CollisionSystemClient::CollisionSystemClient()
{
    m_signature |= 1 << static_cast<U16>(ComponentType::COLLIDER);
}

//----------------------------------------------------------------------------------------------------
bool CollisionSystemClient::DebugRender()
{
    OPTICK_EVENT();

    std::weak_ptr<RendererComponent> rendererComponent = g_gameClient->GetRendererComponent();
    if (!rendererComponent.lock()->m_isDebugDrawColliders) {
        return true;
    }

    for (const auto& entity : m_entities) {
        std::weak_ptr<TransformComponent> transformComponent = g_gameClient->GetComponentManager().GetComponent<TransformComponent>(entity);
        std::weak_ptr<ColliderComponent> colliderComponent = g_gameClient->GetComponentManager().GetComponent<ColliderComponent>(entity);
        DebugDraw(transformComponent, colliderComponent);
    }

    return true;
}
}
