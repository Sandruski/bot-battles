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
void ContactListener::BeginContact(b2Contact* contact)
{
    b2Body* bodyA = contact->GetFixtureA()->GetBody();
    b2Body* bodyB = contact->GetFixtureB()->GetBody();
    Entity entityA = *static_cast<Entity*>(bodyA->GetUserData());
    Entity entityB = *static_cast<Entity*>(bodyB->GetUserData());

    //b2WorldManifold worldManifold;
    //contact->GetWorldManifold(&worldManifold);
    //b2Vec2 linearVelocityA = bodyA->GetLinearVelocityFromWorldPoint(worldManifold.points[0]);
    //b2Vec2 linearVelocityB = bodyB->GetLinearVelocityFromWorldPoint(worldManifold.points[0]);
    //b2Vec2 relativeLinearVelocity = linearVelocityA - linearVelocityB;

    g_game->GetPhysicsComponent().OnCollisionEnter(entityA, entityB);
}

//----------------------------------------------------------------------------------------------------
void ContactListener::EndContact(b2Contact* /*contact*/)
{
    //Entity entityA = *static_cast<Entity*>(contact->GetFixtureA()->GetBody()->GetUserData());
    //Entity entityB = *static_cast<Entity*>(contact->GetFixtureB()->GetBody()->GetUserData());
    //g_game->GetPhysicsComponent().OnCollisionExit();
}

//----------------------------------------------------------------------------------------------------
PhysicsComponent::RaycastHit::RaycastHit()
    : m_entity(INVALID_ENTITY)
    , m_point(0.0f, 0.0f)
    , m_normal(0.0f, 0.0f)
{
}

//----------------------------------------------------------------------------------------------------
PhysicsComponent::Collision::Collision()
    : m_relativeLinearVelocity(0.0f, 0.0f)
{
}

//----------------------------------------------------------------------------------------------------
PhysicsComponent::PhysicsComponent()
    : m_world(b2Vec2(0.0f, 0.0f))
    , m_contactListener()
    , m_timeStep(1.0f / 60.0f)
    , m_velocityIterations(6)
    , m_positionIterations(2)
    , m_epsilon(0.0001f)
{
    m_world.SetContactListener(&m_contactListener);
}

//----------------------------------------------------------------------------------------------------
bool PhysicsComponent::PreUpdate()
{
    NotifyEvents();

    return true;
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

//----------------------------------------------------------------------------------------------------
void PhysicsComponent::OnCollisionEnter(Entity entityA, Entity entityB)
{
    Event newEvent;
    newEvent.eventType = EventType::COLLISION_ENTER;
    newEvent.collision.entityA = entityA;
    newEvent.collision.entityB = entityB;
    PushEvent(newEvent);
}

//----------------------------------------------------------------------------------------------------
void PhysicsComponent::OnCollisionExit(Entity entityA, Entity entityB)
{
    Event newEvent;
    newEvent.eventType = EventType::COLLISION_EXIT;
    newEvent.collision.entityA = entityA;
    newEvent.collision.entityB = entityB;
    PushEvent(newEvent);
}
}
