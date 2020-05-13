#ifndef __PHYSICS_COMPONENT_H__
#define __PHYSICS_COMPONENT_H__

namespace sand {

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
class ContactListener : public b2ContactListener {
public:
    void BeginContact(b2Contact* contact) override;
    void EndContact(b2Contact* contact) override;
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

        glm::vec2 m_relativeLinearVelocity;
    };

    PhysicsComponent();

    bool PreUpdate();

    void Step();
    bool Raycast(const glm::vec2& origin, const glm::vec2& destination, RaycastHit& hitInfo);

    void OnCollisionEnter(Entity entityA, Entity entityB);
    void OnCollisionExit(Entity entityA, Entity entityB);

    b2World m_world;
    ContactListener m_contactListener;
    F32 m_timeStep;
    I32 m_velocityIterations;
    I32 m_positionIterations;
    F32 m_epsilon;
};
}

#endif
