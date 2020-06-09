#include "CollisionSystemServer.h"

#include "ColliderComponent.h"
#include "ComponentManager.h"
#include "ComponentMemberTypes.h"
#include "GameServer.h"
#include "RendererComponent.h"
#include "TransformComponent.h"

namespace sand {

//----------------------------------------------------------------------------------------------------
CollisionSystemServer::CollisionSystemServer()
{
    m_signature |= 1 << static_cast<U16>(ComponentType::COLLIDER);
}

//----------------------------------------------------------------------------------------------------
bool CollisionSystemServer::DebugRender()
{
    OPTICK_EVENT();

    std::weak_ptr<RendererComponent> rendererComponent = g_gameServer->GetRendererComponent();
    if (!rendererComponent.lock()->m_isDebugDrawColliders) {
        return true;
    }

    for (const auto& entity : m_entities) {
        std::weak_ptr<TransformComponent> transformComponent = g_gameServer->GetComponentManager().GetComponent<TransformComponent>(entity);
        std::weak_ptr<ColliderComponent> colliderComponent = g_gameServer->GetComponentManager().GetComponent<ColliderComponent>(entity);
        DebugDraw(transformComponent, colliderComponent);
    }

    return true;
}
}
