#include "RigidbodyComponent.h"

#include "ComponentMemberTypes.h"
#include "Game.h"
#include "PhysicsComponent.h"

namespace sand {

//----------------------------------------------------------------------------------------------------
RigidbodyComponent::RigidbodyComponent()
    : m_body(nullptr)
#ifdef _SERVER
    , m_position(0.0f, 0.0f)
    , m_rotation(0.0f)
#endif
    , m_bodyType(BodyType::NONE)
    , m_groupIndex(0)
{
}

//----------------------------------------------------------------------------------------------------
RigidbodyComponent::~RigidbodyComponent()
{
    if (m_body != nullptr) {
        PhysicsComponent& physicsComponent = g_game->GetPhysicsComponent();
        physicsComponent.m_world.DestroyBody(m_body);
    }
}

#ifdef _CLIENT
//----------------------------------------------------------------------------------------------------
void RigidbodyComponent::Read(InputMemoryStream& inputStream, U32 dirtyState, U32 /*frame*/, ReplicationActionType /*replicationActionType*/, Entity /*entity*/)
{
    if (dirtyState & static_cast<U32>(ComponentMemberType::RIGIDBODY_ENABLED)) {
        inputStream.Read(m_isEnabled);
    }
    if (dirtyState & static_cast<U32>(ComponentMemberType::RIGIDBODY_BODY_TYPE)) {
        inputStream.Read(m_bodyType);
        UpdateBodyType();
    }
    if (dirtyState & static_cast<U32>(ComponentMemberType::RIGIDBODY_GROUP_INDEX)) {
        inputStream.Read(m_groupIndex);
        UpdateGroupIndex();
    }
}
#elif defined(_SERVER)
//----------------------------------------------------------------------------------------------------
U32 RigidbodyComponent::Write(OutputMemoryStream& outputStream, U32 dirtyState) const
{
    U32 writtenState = 0;

    if (dirtyState & static_cast<U32>(ComponentMemberType::RIGIDBODY_ENABLED)) {
        outputStream.Write(m_isEnabled);
        writtenState |= static_cast<U32>(ComponentMemberType::RIGIDBODY_ENABLED);
    }
    if (dirtyState & static_cast<U32>(ComponentMemberType::RIGIDBODY_BODY_TYPE)) {
        outputStream.Write(m_bodyType);
        writtenState |= static_cast<U32>(ComponentMemberType::RIGIDBODY_BODY_TYPE);
    }
    if (dirtyState & static_cast<U32>(ComponentMemberType::RIGIDBODY_GROUP_INDEX)) {
        outputStream.Write(m_groupIndex);
        writtenState |= static_cast<U32>(ComponentMemberType::RIGIDBODY_GROUP_INDEX);
    }

    return writtenState;
}
#endif

//----------------------------------------------------------------------------------------------------
void RigidbodyComponent::SetAsCircle(glm::vec2 position, F32 rotation, F32 radius, const Entity& entity)
{
    if (m_body != nullptr) {
        return;
    }

    b2BodyDef bodyDef;
    bodyDef.userData = const_cast<Entity*>(&entity);
    bodyDef.position.Set(PIXELS_TO_METERS(position.x), PIXELS_TO_METERS(position.y));
    bodyDef.angle = glm::radians(rotation);
    PhysicsComponent& physicsComponent = g_game->GetPhysicsComponent();
    m_body = physicsComponent.m_world.CreateBody(&bodyDef);

    b2FixtureDef fixtureDef;
    b2CircleShape shape;
    shape.m_radius = PIXELS_TO_METERS(radius);
    fixtureDef.shape = &shape;
    if (m_bodyType == BodyType::DYNAMIC) {
        fixtureDef.density = 1.0f;
    }
    m_body->CreateFixture(&fixtureDef);
}

//----------------------------------------------------------------------------------------------------
void RigidbodyComponent::SetAsBox(glm::vec2 position, F32 rotation, glm::vec2 halfSize, const Entity& entity)
{
    if (m_body != nullptr) {
        return;
    }

    b2BodyDef bodyDef;
    bodyDef.userData = const_cast<Entity*>(&entity);
    bodyDef.position.Set(PIXELS_TO_METERS(position.x), PIXELS_TO_METERS(position.y));
    bodyDef.angle = glm::radians(rotation);
    PhysicsComponent& physicsComponent = g_game->GetPhysicsComponent();
    m_body = physicsComponent.m_world.CreateBody(&bodyDef);

    b2FixtureDef fixtureDef;
    b2PolygonShape shape;
    shape.SetAsBox(PIXELS_TO_METERS(halfSize.x), PIXELS_TO_METERS(halfSize.y));
    fixtureDef.shape = &shape;
    if (m_bodyType == BodyType::DYNAMIC) {
        fixtureDef.density = 1.0f;
    }
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
    m_body->GetFixtureList()->SetFilterData(filter);
}
}
