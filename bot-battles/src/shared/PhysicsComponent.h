#ifndef __PHYSICS_COMPONENT_H__
#define __PHYSICS_COMPONENT_H__

namespace sand {

//----------------------------------------------------------------------------------------------------
class ContactListener : public b2ContactListener {
public:
    void BeginContact(b2Contact* contact) override;
    void PreSolve(b2Contact* contact, const b2Manifold* oldManifold) override;
    void EndContact(b2Contact* contact) override;
};

//----------------------------------------------------------------------------------------------------
class QueryCallback : public b2QueryCallback {
public:
    QueryCallback();

    bool ReportFixture(b2Fixture* fixture) override;

public:
    std::vector<b2Body*> m_bodies;
};

//----------------------------------------------------------------------------------------------------
class RayCastCallback : public b2RayCastCallback {
public:
    RayCastCallback();

    float32 ReportFixture(b2Fixture* fixture, const b2Vec2& point,
        const b2Vec2& normal, float32 fraction) override;

public:
    b2Body* m_body;
    b2Vec2 m_point;
    b2Vec2 m_normal;
};

//----------------------------------------------------------------------------------------------------
// System Component
struct PhysicsComponent : public Subject {

    struct RaycastHit {

        RaycastHit();

        Entity m_entity;
        glm::vec2 m_point;
        glm::vec2 m_normal;
    };

    struct Collision {

        Collision();

        // Python
        std::tuple<F32, F32> GetNormal() const;
        std::tuple<F32, F32> GetRelativeVelocity() const;

        glm::vec2 m_normal;
        glm::vec2 m_relativeVelocity;
    };

    PhysicsComponent();

    bool PreUpdate();

    void Step();
    bool Raycast(const glm::vec2& origin, const glm::vec2& destination, RaycastHit& hitInfo);
    bool Overlap(const glm::vec2& center, const glm::vec2& extents, std::vector<Entity>& entities);

    void OnCollisionEnter(Entity entityA, Entity entityB, glm::vec2 linearVelocityA, glm::vec2 linearVelocityB, glm::vec2 normal);
    void OnCollisionExit(Entity entityA, Entity entityB, glm::vec2 linearVelocityA, glm::vec2 linearVelocityB, glm::vec2 normal);

    b2World m_world;
    ContactListener m_contactListener;
    F32 m_timeStep;
    I32 m_velocityIterations;
    I32 m_positionIterations;
    F32 m_epsilon;
};
}

#endif
