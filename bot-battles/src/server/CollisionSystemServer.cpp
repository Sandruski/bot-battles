#include "CollisionSystemServer.h"

#include "ColliderComponent.h"
#include "ComponentManager.h"
#include "ComponentMemberTypes.h"
#include "DebugDrawer.h"
#include "GameServer.h"
#include "TransformComponent.h"

namespace sand {

//----------------------------------------------------------------------------------------------------
    CollisionSystemServer::CollisionSystemServer()
    {
        m_signature |= 1 << static_cast<U16>(ComponentType::COLLIDER);
        m_signature |= 1 << static_cast<U16>(ComponentType::TRANSFORM);
    }

//----------------------------------------------------------------------------------------------------
bool CollisionSystemServer::Update()
{
    for (auto& entity : m_entities) {
        std::weak_ptr<ColliderComponent> colliderComponent = g_gameServer->GetComponentManager().GetComponent<ColliderComponent>(entity);
        std::weak_ptr<TransformComponent> transformComponent = g_gameServer->GetComponentManager().GetComponent<TransformComponent>(entity);

        colliderComponent.lock()->m_position.x = transformComponent.lock()->m_position.x;
        colliderComponent.lock()->m_position.y = transformComponent.lock()->m_position.y;
    }

    return true;
}

//----------------------------------------------------------------------------------------------------
bool CollisionSystemServer::DebugRender()
{
    for (auto& entity : m_entities) {
        std::weak_ptr<ColliderComponent> colliderComponent = g_gameServer->GetComponentManager().GetComponent<ColliderComponent>(entity);
        DebugDrawer::DrawQuad(colliderComponent.lock()->GetRect(), Green, false);
    }

    return true;
}
}
