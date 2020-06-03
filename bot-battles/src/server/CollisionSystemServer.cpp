#include "CollisionSystemServer.h"

#include "ColliderComponent.h"
#include "ComponentManager.h"
#include "ComponentMemberTypes.h"
#include "GameServer.h"
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

    for (const auto& entity : m_entities) {
        std::weak_ptr<TransformComponent> transformComponent = g_gameServer->GetComponentManager().GetComponent<TransformComponent>(entity);
        std::weak_ptr<ColliderComponent> colliderComponent = g_gameServer->GetComponentManager().GetComponent<ColliderComponent>(entity);
        DebugDraw(transformComponent, colliderComponent);
    }

    return true;
}
}
