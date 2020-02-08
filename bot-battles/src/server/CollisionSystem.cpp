#include "CollisionSystem.h"

#include "ColliderComponent.h"
#include "ComponentManager.h"
#include "ComponentMemberTypes.h"
#include "DebugDrawer.h"
#include "GameServer.h"
#include "TransformComponent.h"

namespace sand {

//----------------------------------------------------------------------------------------------------
CollisionSystem::CollisionSystem()
{
    m_signature |= 1 << static_cast<U16>(ComponentType::COLLIDER);
    m_signature |= 1 << static_cast<U16>(ComponentType::TRANSFORM);
}

//----------------------------------------------------------------------------------------------------
bool CollisionSystem::Update()
{
    for (auto& entity : m_entities) {
        std::weak_ptr<ColliderComponent> colliderComponent = g_gameServer->GetComponentManager().GetComponent<ColliderComponent>(entity);
        std::weak_ptr<TransformComponent> transformComponent = g_gameServer->GetComponentManager().GetComponent<TransformComponent>(entity);

        colliderComponent.lock()->m_position.x = transformComponent.lock()->m_position.x;
        colliderComponent.lock()->m_position.y = transformComponent.lock()->m_position.y;

        Event newEvent;
        newEvent.eventType = EventType::COMPONENT_MEMBER_CHANGED;
        newEvent.component.entity = entity;
        newEvent.component.dirtyState = static_cast<U32>(ComponentMemberType::COLLIDER_POSITION);
        NotifyEvent(newEvent);
    }

    return true;
}

//----------------------------------------------------------------------------------------------------
bool CollisionSystem::DebugRender()
{
    for (auto& entity : m_entities) {
        std::weak_ptr<ColliderComponent> colliderComponent = g_gameServer->GetComponentManager().GetComponent<ColliderComponent>(entity);
        SDL_Rect colliderQuad = {
            static_cast<I32>(colliderComponent.lock()->m_position.x - colliderComponent.lock()->m_size.x / 2),
            static_cast<I32>(colliderComponent.lock()->m_position.y - colliderComponent.lock()->m_size.y / 2),
            static_cast<I32>(colliderComponent.lock()->m_size.x),
            static_cast<I32>(colliderComponent.lock()->m_size.y),
        };
        DebugDrawer::DrawQuad(colliderQuad, Green, false);
    }

    return true;
}
}
