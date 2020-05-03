#include "PhysicsComponent.h"

#include "ComponentManager.h"
#include "Game.h"
#include "RigidbodyComponent.h"

namespace sand {

//----------------------------------------------------------------------------------------------------
RayCastCallback::RayCastCallback()
    : m_body(nullptr)
    , m_point(0.0f, 0.0f)
    , m_normal(0.0f, 0.0f)
{
}

//----------------------------------------------------------------------------------------------------
float32 RayCastCallback::ReportFixture(b2Fixture* fixture, const b2Vec2& point, const b2Vec2& normal, float32 fraction)
{
    m_body = fixture->GetBody();
    m_point = point;
    m_normal = normal;

    return fraction;
}

//----------------------------------------------------------------------------------------------------
PhysicsComponent::RaycastHit::RaycastHit()
    : m_entity(INVALID_ENTITY)
    , m_point(0.0f, 0.0f)
    , m_normal(0.0f, 0.0f)
{
}

//----------------------------------------------------------------------------------------------------
PhysicsComponent::PhysicsComponent()
    : m_world(b2Vec2(0.0f, 0.0f))
    , m_timeStep(1.0f) // 1.0f / 60.0f
    , m_velocityIterations(6)
    , m_positionIterations(2)
    , m_epsilon(b2_epsilon)
{
}

//----------------------------------------------------------------------------------------------------
void PhysicsComponent::Step()
{
    m_world.Step(m_timeStep, m_velocityIterations, m_positionIterations);
}

//----------------------------------------------------------------------------------------------------
bool PhysicsComponent::Raycast(const glm::vec2& origin, const glm::vec2& destination, RaycastHit& hitInfo) const
{
    RayCastCallback rayCastCallback;
    m_world.RayCast(&rayCastCallback, b2Vec2(origin.x, origin.y), b2Vec2(destination.x, destination.y));
    if (rayCastCallback.m_body != nullptr) {
        /*
        std::vector<std::pair<Entity, std::weak_ptr<RigidbodyComponent>>> rigidbodyComponents = g_game->GetComponentManager().GetComponents<RigidbodyComponent>();
        for (const auto& pair : rigidbodyComponents) {
            std::weak_ptr<RigidbodyComponent> rigidbodyComponent = pair.second;
            if (rayCastCallback.m_body == rigidbodyComponent.lock()->m_body) {
                Entity entity = pair.first;
                hitInfo.m_entity = entity;
                break;
            }
        }*/
        hitInfo.m_entity = *static_cast<Entity*>(rayCastCallback.m_body->GetUserData());
        hitInfo.m_point = glm::vec2(rayCastCallback.m_point.x, rayCastCallback.m_point.y);
        hitInfo.m_normal = glm::vec2(rayCastCallback.m_normal.x, rayCastCallback.m_normal.y);

        return true;
    }

    return false;
}
}
