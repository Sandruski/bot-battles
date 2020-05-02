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
// System Component
struct PhysicsComponent {

    struct RaycastHit {

        RaycastHit();

        Entity m_entity;
        glm::vec2 m_point;
        glm::vec2 m_normal;
    };

    PhysicsComponent();

    bool Raycast(const glm::vec2& origin, const glm::vec2& destination, RaycastHit& hitInfo) const;

    b2World m_world;
    F32 m_timeStep;
    I32 m_velocityIterations;
    I32 m_positionIterations;
};
}

#endif
