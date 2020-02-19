#include "CollisionSystemClient.h"

#include "ColliderComponent.h"
#include "ComponentManager.h"
#include "ComponentMemberTypes.h"
#include "DebugDrawer.h"
#include "GameClient.h"
#include "LinkingContext.h"
#include "TransformComponent.h"

namespace sand {

//----------------------------------------------------------------------------------------------------
CollisionSystemClient::CollisionSystemClient()
{
    m_signature |= 1 << static_cast<U16>(ComponentType::COLLIDER);
    m_signature |= 1 << static_cast<U16>(ComponentType::TRANSFORM);
}

//----------------------------------------------------------------------------------------------------
bool CollisionSystemClient::Update()
{
    for (auto& entity : m_entities) {
        std::weak_ptr<ColliderComponent> colliderComponent = g_gameClient->GetComponentManager().GetComponent<ColliderComponent>(entity);
        std::weak_ptr<TransformComponent> transformComponent = g_gameClient->GetComponentManager().GetComponent<TransformComponent>(entity);

        colliderComponent.lock()->m_position.x = transformComponent.lock()->m_position.x;
        colliderComponent.lock()->m_position.y = transformComponent.lock()->m_position.y;
    }

    return true;
}

//----------------------------------------------------------------------------------------------------
bool CollisionSystemClient::DebugRender()
{
    for (auto& entity : m_entities) {
        std::weak_ptr<ColliderComponent> colliderComponent = g_gameClient->GetComponentManager().GetComponent<ColliderComponent>(entity);
        DebugDrawer::DrawQuad(colliderComponent.lock()->GetRect(), Green, false);

        if (g_gameClient->GetLinkingContext().GetNetworkID(entity) >= INVALID_NETWORK_ID) {
            continue;
        }

        DebugDrawer::DrawQuad(colliderComponent.lock()->GetShotRect(), Orange, false);
    }

    return true;
}
}
