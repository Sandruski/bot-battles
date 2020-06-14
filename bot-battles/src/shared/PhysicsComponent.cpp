#include "PhysicsComponent.h"

#include "ColliderComponent.h"
#include "ComponentManager.h"
#include "Game.h"
#include "RigidbodyComponent.h"
#include "WallComponent.h"

namespace sand {

//----------------------------------------------------------------------------------------------------
void ContactListener::BeginContact(b2Contact* contact)
{
    b2Body* bodyA = contact->GetFixtureA()->GetBody();
    b2Body* bodyB = contact->GetFixtureB()->GetBody();
    Entity entityA = *static_cast<Entity*>(bodyA->GetUserData());
    Entity entityB = *static_cast<Entity*>(bodyB->GetUserData());

    b2WorldManifold worldManifold;
    contact->GetWorldManifold(&worldManifold);
    b2Vec2 physicsLinearVelocityA = bodyA->GetLinearVelocityFromWorldPoint(worldManifold.points[0]);
    b2Vec2 physicsLinearVelocityB = bodyB->GetLinearVelocityFromWorldPoint(worldManifold.points[0]);
    glm::vec2 linearVelocityA = glm::vec2(METERS_TO_PIXELS(physicsLinearVelocityA.x), METERS_TO_PIXELS(physicsLinearVelocityA.y));
    glm::vec2 linearVelocityB = glm::vec2(METERS_TO_PIXELS(physicsLinearVelocityB.x), METERS_TO_PIXELS(physicsLinearVelocityB.y));
    glm::vec2 normal = glm::vec2(worldManifold.normal.x, worldManifold.normal.y);

    g_game->GetPhysicsComponent().lock()->OnCollisionEnter(entityA, entityB, linearVelocityA, linearVelocityB, normal);
}

//----------------------------------------------------------------------------------------------------
void ContactListener::PreSolve(b2Contact* contact, const b2Manifold* /*oldManifold*/)
{
    b2Body* bodyA = contact->GetFixtureA()->GetBody();
    b2Body* bodyB = contact->GetFixtureB()->GetBody();
    Entity entityA = *static_cast<Entity*>(bodyA->GetUserData());
    Entity entityB = *static_cast<Entity*>(bodyB->GetUserData());

    std::weak_ptr<ColliderComponent> colliderComponentA = g_game->GetComponentManager().AddComponent<ColliderComponent>(entityA);
    std::weak_ptr<ColliderComponent> colliderComponentB = g_game->GetComponentManager().AddComponent<ColliderComponent>(entityB);
    if ((!colliderComponentA.expired() && colliderComponentA.lock()->m_isTrigger)
        || (!colliderComponentB.expired() && colliderComponentB.lock()->m_isTrigger)) {
        contact->SetEnabled(false);
    }
}

//----------------------------------------------------------------------------------------------------
void ContactListener::EndContact(b2Contact* contact)
{
    b2Body* bodyA = contact->GetFixtureA()->GetBody();
    b2Body* bodyB = contact->GetFixtureB()->GetBody();
    Entity entityA = *static_cast<Entity*>(bodyA->GetUserData());
    Entity entityB = *static_cast<Entity*>(bodyB->GetUserData());

    b2WorldManifold worldManifold;
    contact->GetWorldManifold(&worldManifold);
    b2Vec2 physicsLinearVelocityA = bodyA->GetLinearVelocityFromWorldPoint(worldManifold.points[0]);
    b2Vec2 physicsLinearVelocityB = bodyB->GetLinearVelocityFromWorldPoint(worldManifold.points[0]);
    glm::vec2 linearVelocityA = glm::vec2(METERS_TO_PIXELS(physicsLinearVelocityA.x), METERS_TO_PIXELS(physicsLinearVelocityA.y));
    glm::vec2 linearVelocityB = glm::vec2(METERS_TO_PIXELS(physicsLinearVelocityB.x), METERS_TO_PIXELS(physicsLinearVelocityB.y));
    glm::vec2 normal = glm::vec2(worldManifold.normal.x, worldManifold.normal.y);

    g_game->GetPhysicsComponent().lock()->OnCollisionExit(entityA, entityB, linearVelocityA, linearVelocityB, normal);
}

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
QueryCallback::QueryCallback()
    : m_bodies()
{
}

//----------------------------------------------------------------------------------------------------
bool QueryCallback::ReportFixture(b2Fixture* fixture)
{
    b2Body* body = fixture->GetBody();
    m_bodies.emplace_back(body);
    return true;
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
    : m_normal(0.0f, 0.0f)
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
    m_world.SetAllowSleeping(false);
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
bool PhysicsComponent::Raycast(const glm::vec2& origin, const glm::vec2& destination, Entity entity, RaycastHit& hitInfo)
{
    bool ret = false;

    for (b2Body* body = m_world.GetBodyList(); body != nullptr; body = body->GetNext()) {
        b2BodyType type = body->GetType();
        switch (type) {
        case b2BodyType::b2_staticBody: {
            Entity bodyEntity = *static_cast<Entity*>(body->GetUserData());
            if (bodyEntity == entity) {
                continue;
            }

            std::weak_ptr<WallComponent> wallComponent = g_game->GetComponentManager().GetComponent<WallComponent>(bodyEntity);
            if (wallComponent.expired()) {
                body->SetActive(false);
            }
            break;
        }
        case b2BodyType::b2_dynamicBody: {
            body->SetActive(true);
            break;
        }
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
        switch (type) {
        case b2BodyType::b2_staticBody: {
            Entity bodyEntity = *static_cast<Entity*>(body->GetUserData());
            if (bodyEntity == entity) {
                continue;
            }

            std::weak_ptr<WallComponent> wallComponent = g_game->GetComponentManager().GetComponent<WallComponent>(bodyEntity);
            if (wallComponent.expired()) {
                body->SetActive(true);
            }
            break;
        }
        case b2BodyType::b2_dynamicBody: {
            body->SetActive(false);
            break;
        }
        }
    }

    return ret;
}

//----------------------------------------------------------------------------------------------------
bool PhysicsComponent::Overlap(const glm::vec2& center, const glm::vec2& extents, std::vector<Entity>& entities)
{
    bool ret = false;

    for (b2Body* body = m_world.GetBodyList(); body != nullptr; body = body->GetNext()) {
        b2BodyType type = body->GetType();
        if (type == b2BodyType::b2_dynamicBody) {
            body->SetActive(true);
        }
    }

    b2AABB aabb;
    glm::vec2 lowerBound = center - extents;
    aabb.lowerBound = b2Vec2(PIXELS_TO_METERS(lowerBound.x), PIXELS_TO_METERS(lowerBound.y));
    glm::vec2 upperBound = center + extents;
    aabb.upperBound = b2Vec2(PIXELS_TO_METERS(upperBound.x), PIXELS_TO_METERS(upperBound.y));

    QueryCallback queryCallback;
    m_world.QueryAABB(&queryCallback, aabb);
    if (!queryCallback.m_bodies.empty()) {
        for (const auto& body : queryCallback.m_bodies) {
            Entity entity = *static_cast<Entity*>(body->GetUserData());
            entities.push_back(entity);
        }

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
void PhysicsComponent::OnCollisionEnter(Entity entityA, Entity entityB, glm::vec2 linearVelocityA, glm::vec2 linearVelocityB, glm::vec2 normal)
{
    Event newEvent;
    newEvent.eventType = EventType::COLLISION_ENTER;
    newEvent.collision.entityA = entityA;
    newEvent.collision.entityB = entityB;
    newEvent.collision.linearVelocityA = linearVelocityA;
    newEvent.collision.linearVelocityB = linearVelocityB;
    newEvent.collision.normal = normal;
    PushEvent(newEvent);
}

//----------------------------------------------------------------------------------------------------
void PhysicsComponent::OnCollisionExit(Entity entityA, Entity entityB, glm::vec2 linearVelocityA, glm::vec2 linearVelocityB, glm::vec2 normal)
{
    Event newEvent;
    newEvent.eventType = EventType::COLLISION_EXIT;
    newEvent.collision.entityA = entityA;
    newEvent.collision.entityB = entityB;
    newEvent.collision.linearVelocityA = linearVelocityA;
    newEvent.collision.linearVelocityB = linearVelocityB;
    newEvent.collision.normal = normal;
    PushEvent(newEvent);
}
}
