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
    , m_timeStep(1.0f / 60.0f)
    , m_velocityIterations(6)
    , m_positionIterations(2)
    , m_epsilon(0.0001f)
{
}

//----------------------------------------------------------------------------------------------------
void PhysicsComponent::Step()
{
    m_world.Step(m_timeStep, m_velocityIterations, m_positionIterations);
}

//----------------------------------------------------------------------------------------------------
bool PhysicsComponent::Raycast(const glm::vec2& origin, const glm::vec2& destination, RaycastHit& hitInfo)
{
    bool ret = false;

    for (b2Body* body = m_world.GetBodyList(); body != nullptr; body = body->GetNext()) {
        b2BodyType type = body->GetType();
        if (type == b2BodyType::b2_dynamicBody) {
            body->SetActive(true);
        }
    }

    RayCastCallback rayCastCallback;
    m_world.RayCast(&rayCastCallback, b2Vec2(PIXELS_TO_METERS(origin.x), PIXELS_TO_METERS(origin.y)), b2Vec2(PIXELS_TO_METERS(destination.x), PIXELS_TO_METERS(destination.y)));
    if (rayCastCallback.m_body != nullptr) {
        hitInfo.m_entity = *static_cast<Entity*>(rayCastCallback.m_body->GetUserData());
        hitInfo.m_point = glm::vec2(METERS_TO_PIXELS(rayCastCallback.m_point.x), METERS_TO_PIXELS(rayCastCallback.m_point.y));
        hitInfo.m_normal = glm::vec2(METERS_TO_PIXELS(rayCastCallback.m_normal.x), METERS_TO_PIXELS(rayCastCallback.m_normal.y));

        ret = true;
    }

    for (b2Body* body = m_world.GetBodyList(); body != nullptr; body = body->GetNext()) {
        b2BodyType type = body->GetType();
        if (type == b2BodyType::b2_dynamicBody) {
            body->SetActive(false);
        }
    }

    return ret;
}
}
