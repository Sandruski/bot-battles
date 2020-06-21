#include "RigidbodyComponent.h"

#include "ComponentMemberTypes.h"
#include "Game.h"
#include "PhysicsComponent.h"

namespace sand {

//----------------------------------------------------------------------------------------------------
RigidbodyComponent::RigidbodyComponent()
    : m_maxLinearVelocity(0.0f)
    , m_maxAngularVelocity(0.0f)
    , m_bodyType(BodyType::NONE)
    , m_groupIndex(0)
    , m_isBullet(false)
    , m_body(nullptr)
{
}

//----------------------------------------------------------------------------------------------------
RigidbodyComponent::~RigidbodyComponent()
{
    if (m_body != nullptr) {
        std::weak_ptr<PhysicsComponent> physicsComponent = g_game->GetPhysicsComponent();
        physicsComponent.lock()->m_world.DestroyBody(m_body);
    }
}

#ifdef _CLIENT
//----------------------------------------------------------------------------------------------------
void RigidbodyComponent::Read(InputMemoryStream& inputStream, U64 dirtyState, U32 /*frame*/, ReplicationActionType /*replicationActionType*/, Entity /*entity*/)
{
    if (dirtyState & static_cast<U64>(ComponentMemberType::RIGIDBODY_MAX_LINEAR_VELOCITY)) {
        inputStream.Read(m_maxLinearVelocity);
    }
    if (dirtyState & static_cast<U64>(ComponentMemberType::RIGIDBODY_MAX_ANGULAR_VELOCITY)) {
        inputStream.Read(m_maxAngularVelocity);
    }
    if (dirtyState & static_cast<U64>(ComponentMemberType::RIGIDBODY_BODY_TYPE)) {
        inputStream.Read(m_bodyType);
        UpdateBodyType();
    }
    if (dirtyState & static_cast<U64>(ComponentMemberType::RIGIDBODY_GROUP_INDEX)) {
        inputStream.Read(m_groupIndex);
        UpdateGroupIndex();
    }
    if (dirtyState & static_cast<U64>(ComponentMemberType::RIGIDBODY_BULLET)) {
        inputStream.Read(m_isBullet);
        UpdateBullet();
    }
}
#elif defined(_SERVER)
//----------------------------------------------------------------------------------------------------
U64 RigidbodyComponent::Write(OutputMemoryStream& outputStream, U64 dirtyState) const
{
    U64 writtenState = 0;

    if (dirtyState & static_cast<U64>(ComponentMemberType::RIGIDBODY_MAX_LINEAR_VELOCITY)) {
        outputStream.Write(m_maxLinearVelocity);
        writtenState |= static_cast<U64>(ComponentMemberType::RIGIDBODY_MAX_LINEAR_VELOCITY);
    }
    if (dirtyState & static_cast<U64>(ComponentMemberType::RIGIDBODY_MAX_ANGULAR_VELOCITY)) {
        outputStream.Write(m_maxAngularVelocity);
        writtenState |= static_cast<U64>(ComponentMemberType::RIGIDBODY_MAX_ANGULAR_VELOCITY);
    }
    if (dirtyState & static_cast<U64>(ComponentMemberType::RIGIDBODY_BODY_TYPE)) {
        outputStream.Write(m_bodyType);
        writtenState |= static_cast<U64>(ComponentMemberType::RIGIDBODY_BODY_TYPE);
    }
    if (dirtyState & static_cast<U64>(ComponentMemberType::RIGIDBODY_GROUP_INDEX)) {
        outputStream.Write(m_groupIndex);
        writtenState |= static_cast<U64>(ComponentMemberType::RIGIDBODY_GROUP_INDEX);
    }
    if (dirtyState & static_cast<U64>(ComponentMemberType::RIGIDBODY_BULLET)) {
        outputStream.Write(m_isBullet);
        writtenState |= static_cast<U64>(ComponentMemberType::RIGIDBODY_BULLET);
    }

    return writtenState;
}
#endif

//----------------------------------------------------------------------------------------------------
void RigidbodyComponent::SetAsCircle(const glm::vec2& position, F32 rotation, F32 radius, const Entity& entity)
{
    if (m_body != nullptr) {
        return;
    }

    b2BodyDef bodyDef;
    bodyDef.linearDamping = 0.0f;
    bodyDef.angularDamping = 0.0f;
    bodyDef.gravityScale = 0.0f;
    bodyDef.fixedRotation = true;
    bodyDef.userData = const_cast<Entity*>(&entity);
    bodyDef.position.Set(PIXELS_TO_METERS(position.x), PIXELS_TO_METERS(position.y));
    bodyDef.angle = glm::radians(rotation);
    std::weak_ptr<PhysicsComponent> physicsComponent = g_game->GetPhysicsComponent();
    m_body = physicsComponent.lock()->m_world.CreateBody(&bodyDef);

    b2FixtureDef fixtureDef;
    b2CircleShape shape;
    shape.m_radius = PIXELS_TO_METERS(radius);
    fixtureDef.shape = &shape;
    fixtureDef.restitution = 0.0f;
    fixtureDef.friction = 0.0f;
    fixtureDef.density = 1.0f;
    m_body->CreateFixture(&fixtureDef);
}

//----------------------------------------------------------------------------------------------------
void RigidbodyComponent::SetAsBox(const glm::vec2& position, F32 rotation, const glm::vec2& halfSize, const Entity& entity)
{
    if (m_body != nullptr) {
        return;
    }

    b2BodyDef bodyDef;
    bodyDef.linearDamping = 0.0f;
    bodyDef.angularDamping = 0.0f;
    bodyDef.gravityScale = 0.0f;
    bodyDef.fixedRotation = true;
    bodyDef.userData = const_cast<Entity*>(&entity);
    bodyDef.position.Set(PIXELS_TO_METERS(position.x), PIXELS_TO_METERS(position.y));
    bodyDef.angle = glm::radians(rotation);
    std::weak_ptr<PhysicsComponent> physicsComponent = g_game->GetPhysicsComponent();
    m_body = physicsComponent.lock()->m_world.CreateBody(&bodyDef);

    b2FixtureDef fixtureDef;
    b2PolygonShape shape;
    shape.SetAsBox(PIXELS_TO_METERS(halfSize.x), PIXELS_TO_METERS(halfSize.y));
    fixtureDef.shape = &shape;
    fixtureDef.restitution = 0.0f;
    fixtureDef.friction = 0.0f;
    fixtureDef.density = 1.0f;
    m_body->CreateFixture(&fixtureDef);
}

//----------------------------------------------------------------------------------------------------
void RigidbodyComponent::UpdateBodyType() const
{
    if (m_body == nullptr) {
        return;
    }

    b2BodyType bodyType = b2BodyType::b2_staticBody;

    switch (m_bodyType) {
    case BodyType::STATIC: {
        bodyType = b2BodyType::b2_staticBody;
        break;
    }

    case BodyType::DYNAMIC: {
        bodyType = b2BodyType::b2_dynamicBody;
        break;
    }

    case BodyType::KINEMATIC: {
        bodyType = b2BodyType::b2_kinematicBody;
        break;
    }

    default: {
        break;
    }
    }

    m_body->SetType(bodyType);
}

//----------------------------------------------------------------------------------------------------
void RigidbodyComponent::UpdateGroupIndex() const
{
    if (m_body == nullptr) {
        return;
    }

    b2Filter filter;
    filter.groupIndex = m_groupIndex;
    for (b2Fixture* fixture = m_body->GetFixtureList(); fixture != nullptr; fixture = fixture->GetNext()) {
        fixture->SetFilterData(filter);
    }
}

//----------------------------------------------------------------------------------------------------
void RigidbodyComponent::UpdateBullet() const
{
    if (m_body == nullptr) {
        return;
    }

    m_body->SetBullet(m_isBullet);
}

//----------------------------------------------------------------------------------------------------
glm::vec2 RigidbodyComponent::GetLinearVelocity() const
{
    glm::vec2 linearVelocity = glm::vec2(0.0f, 0.0f);

    if (m_body != nullptr) {
        b2Vec2 physicsLinearVelocity = m_body->GetLinearVelocity();
        linearVelocity = glm::vec2(METERS_TO_PIXELS(physicsLinearVelocity.x), METERS_TO_PIXELS(physicsLinearVelocity.y));
    }

    return linearVelocity;
}

//----------------------------------------------------------------------------------------------------
F32 RigidbodyComponent::GetAngularVelocity() const
{
    F32 angularVelocity = 0.0f;

    if (m_body != nullptr) {
        F32 physicsAngularVelocity = m_body->GetAngularVelocity();
        angularVelocity = glm::degrees(physicsAngularVelocity);
    }

    return angularVelocity;
}
}
