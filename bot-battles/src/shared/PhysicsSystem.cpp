#include "PhysicsSystem.h"

#include "ColliderComponent.h"
#include "ComponentManager.h"
#include "EntityManager.h"
#include "Game.h"
#include "PhysicsComponent.h"
#include "RigidbodyComponent.h"
#include "TransformComponent.h"

namespace sand {

//----------------------------------------------------------------------------------------------------
PhysicsSystem::PhysicsSystem()
{
    m_signature |= 1 << static_cast<U16>(ComponentType::TRANSFORM);
    m_signature |= 1 << static_cast<U16>(ComponentType::COLLIDER);
    m_signature |= 1 << static_cast<U16>(ComponentType::RIGIDBODY);
}

//----------------------------------------------------------------------------------------------------
bool PhysicsSystem::Update()
{
    PhysicsComponent& physicsComponent = g_game->GetPhysicsComponent();

    physicsComponent.m_world.Step(physicsComponent.m_timeStep, physicsComponent.m_velocityIterations, physicsComponent.m_positionIterations);

    return true;
}

//----------------------------------------------------------------------------------------------------
void PhysicsSystem::OnNotify(const Event& event)
{
    switch (event.eventType) {

    case EventType::COMPONENT_ADDED: {
        if (event.component.componentType == ComponentType::RIGIDBODY) {
            OnRigidbodyComponentAdded(event.component.entity);
        }
        break;
    }

    default: {
        break;
    }
    }
}

//----------------------------------------------------------------------------------------------------
void PhysicsSystem::OnRigidbodyComponentAdded(Entity entity) const
{
    std::weak_ptr<TransformComponent> transformComponent = g_game->GetComponentManager().GetComponent<TransformComponent>(entity);
    std::weak_ptr<ColliderComponent> colliderComponent = g_game->GetComponentManager().GetComponent<ColliderComponent>(entity);
    std::weak_ptr<RigidbodyComponent> rigidbodyComponent = g_game->GetComponentManager().GetComponent<RigidbodyComponent>(entity);
    const Entity& originalEntity = g_game->GetEntityManager().GetEntity(entity);

    switch (colliderComponent.lock()->m_shapeType) {
    case ColliderComponent::ShapeType::CIRCLE: {
        rigidbodyComponent.lock()->SetAsCircle(transformComponent.lock()->m_position, transformComponent.lock()->m_rotation, colliderComponent.lock()->m_size.x / 2.0f, originalEntity);
        break;
    }

    case ColliderComponent::ShapeType::BOX: {
        rigidbodyComponent.lock()->SetAsBox(transformComponent.lock()->m_position, transformComponent.lock()->m_rotation, colliderComponent.lock()->m_size / 2.0f, originalEntity);
        break;
    }

    default: {
        break;
    }
    }
}
}
