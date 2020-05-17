#include "WeaponSystemServer.h"

#include "ClientProxy.h"
#include "ComponentManager.h"
#include "ComponentMemberTypes.h"
#include "DebugDrawer.h"
#include "GameServer.h"
#include "HealthComponent.h"
#include "LinkingContext.h"
#include "MeshResource.h"
#include "PhysicsComponent.h"
#include "RendererComponent.h"
#include "RigidbodyComponent.h"
#include "ServerComponent.h"
#include "ShaderResource.h"
#include "State.h"
#include "SystemManager.h"
#include "TransformComponent.h"
#include "WeaponComponent.h"
#include "WindowComponent.h"

namespace sand {
//----------------------------------------------------------------------------------------------------
WeaponSystemServer::WeaponSystemServer()
{
    m_signature |= 1 << static_cast<U16>(ComponentType::TRANSFORM);
    m_signature |= 1 << static_cast<U16>(ComponentType::RIGIDBODY);
    m_signature |= 1 << static_cast<U16>(ComponentType::WEAPON);
    m_signature |= 1 << static_cast<U16>(ComponentType::PLAYER);
}

//----------------------------------------------------------------------------------------------------
bool WeaponSystemServer::PreUpdate()
{
    NotifyEvents();

    return true;
}

//----------------------------------------------------------------------------------------------------
bool WeaponSystemServer::Update()
{
    GameplayComponent& gameplayComponent = g_gameServer->GetGameplayComponent();
    std::weak_ptr<State> currentState = gameplayComponent.m_fsm.GetCurrentState();
    if (currentState.expired()) {
        return true;
    }

    ServerComponent& serverComponent = g_gameServer->GetServerComponent();

    for (auto& entity : m_entities) {
        PlayerID playerID = serverComponent.GetPlayerID(entity);
        if (playerID >= INVALID_PLAYER_ID) {
            continue;
        }

        std::weak_ptr<TransformComponent> transformComponent = g_gameServer->GetComponentManager().GetComponent<TransformComponent>(entity);
        std::weak_ptr<RigidbodyComponent> rigidbodyComponent = g_gameServer->GetComponentManager().GetComponent<RigidbodyComponent>(entity);
        std::weak_ptr<WeaponComponent> weaponComponent = g_gameServer->GetComponentManager().GetComponent<WeaponComponent>(entity);
        if (!transformComponent.lock()->m_isEnabled || !rigidbodyComponent.lock()->m_isEnabled || !weaponComponent.lock()->m_isEnabled) {
            continue;
        }

        std::weak_ptr<ClientProxy> clientProxy = serverComponent.GetClientProxy(playerID);
        for (U32 i = clientProxy.lock()->m_inputBuffer.m_front; i < clientProxy.lock()->m_inputBuffer.m_back; ++i) {
            const Input& input = clientProxy.lock()->m_inputBuffer.Get(i);
            U32 dirtyState = input.GetDirtyState();

            const bool hasShooting = dirtyState & static_cast<U32>(InputComponentMemberType::INPUT_SHOOTING);
            if (hasShooting) {
                if (serverComponent.m_isServerRewind) {
                    Rewind(entity, input.m_interpolationFromFrame, input.m_interpolationToFrame, input.m_interpolationPercentage);
                }

                U32 index = transformComponent.lock()->m_inputTransformBuffer.m_front;
                bool isFound = false;
                while (index < transformComponent.lock()->m_inputTransformBuffer.m_back) {
                    const Transform& transform = transformComponent.lock()->m_inputTransformBuffer.Get(index);
                    if (transform.GetFrame() == input.GetFrame()) {
                        isFound = true;
                        break;
                    }
                    ++index;
                }
                Transform& transform = isFound ? transformComponent.lock()->m_inputTransformBuffer.Get(index) : transformComponent.lock()->m_inputTransformBuffer.GetLast();
                rigidbodyComponent.lock()->m_body->SetTransform(b2Vec2(PIXELS_TO_METERS(transform.m_position.x), PIXELS_TO_METERS(transform.m_position.y)), glm::radians(transform.m_rotation));
                ILOG("SERVER LOCAL REWIND POS %f %f ROT %f", transform.m_position.x, transform.m_position.y, transform.m_rotation);

                glm::vec2 position = transform.m_position;
                F32 x = std::cos(glm::radians(transform.m_rotation));
                F32 y = std::sin(glm::radians(transform.m_rotation));
                glm::vec2 rotation = glm::vec2(x, y);

                weaponComponent.lock()->m_origin = position;
                WindowComponent& windowComponent = g_gameServer->GetWindowComponent();
                F32 maxLength = static_cast<F32>(std::max(windowComponent.m_currentResolution.x, windowComponent.m_currentResolution.y));
                weaponComponent.lock()->m_destination = position + rotation * maxLength;
                weaponComponent.lock()->m_hasHit = false;

                PhysicsComponent& physicsComponent = g_gameServer->GetPhysicsComponent();
                PhysicsComponent::RaycastHit hitInfo;
                const bool hasIntersected = physicsComponent.Raycast(weaponComponent.lock()->m_origin, weaponComponent.lock()->m_destination, hitInfo);
                if (hasIntersected) {
                    std::weak_ptr<TransformComponent> hitEntityTransformComponent = g_gameServer->GetComponentManager().GetComponent<TransformComponent>(hitInfo.m_entity);
                    if (!hitEntityTransformComponent.expired()) {
                        weaponComponent.lock()->m_destination = hitInfo.m_point;
                    }

                    if (hitInfo.m_entity != entity) {
                        std::weak_ptr<HealthComponent> hitEntityHealthComponent = g_gameServer->GetComponentManager().GetComponent<HealthComponent>(hitInfo.m_entity);
                        if (!hitEntityHealthComponent.expired()) {
                            weaponComponent.lock()->m_hasHit = true;

                            Event newEvent;
                            newEvent.eventType = EventType::WEAPON_HIT;
                            newEvent.weapon.entity = entity;
                            newEvent.weapon.damage = weaponComponent.lock()->m_damage;
                            PushEvent(newEvent);
                        }
                    }
                }

                Event newEvent;
                newEvent.eventType = EventType::COMPONENT_MEMBER_CHANGED;
                newEvent.component.entity = entity;
                newEvent.component.dirtyState = static_cast<U32>(ComponentMemberType::WEAPON_ALL);
                NotifyEvent(newEvent);

                rigidbodyComponent.lock()->m_body->SetTransform(b2Vec2(PIXELS_TO_METERS(transformComponent.lock()->m_position.x), PIXELS_TO_METERS(transformComponent.lock()->m_position.y)), glm::radians(transformComponent.lock()->m_rotation));

                if (serverComponent.m_isServerRewind) {
                    Revert(entity);
                }
            }
        }
    }

    return true;
}

//----------------------------------------------------------------------------------------------------
bool WeaponSystemServer::Render()
{
    ServerComponent& serverComponent = g_gameServer->GetServerComponent();
    RendererComponent& rendererComponent = g_gameServer->GetRendererComponent();
    WindowComponent& windowComponent = g_gameServer->GetWindowComponent();
    glm::vec2 proportion = windowComponent.GetProportion();

    for (auto& entity : m_entities) {
        PlayerID playerID = serverComponent.GetPlayerID(entity);
        if (playerID >= INVALID_PLAYER_ID) {
            continue;
        }

        std::weak_ptr<TransformComponent> transformComponent = g_gameServer->GetComponentManager().GetComponent<TransformComponent>(entity);
        std::weak_ptr<WeaponComponent> weaponComponent = g_gameServer->GetComponentManager().GetComponent<WeaponComponent>(entity);
        if (!transformComponent.lock()->m_isEnabled || !weaponComponent.lock()->m_isEnabled) {
            continue;
        }

        glm::vec4 color = White;
        switch (playerID) {
        case 0: {
            color = Red;
            break;
        }
        case 1: {
            color = Blue;
            break;
        }
        default: {
            break;
        }
        }

        glm::mat4 model = glm::mat4(1.0f);

        std::array<MeshResource::Vertex, 4> vertices = MeshResource::GetQuadVertices();
        // From
        glm::vec2 from = weaponComponent.lock()->m_origin;
        from.x *= proportion.x;
        from.y *= proportion.y;
        from.y *= -1.0f;
        vertices[0].m_position = from;
        // To
        glm::vec2 to = weaponComponent.lock()->m_destination;
        to.x *= proportion.x;
        to.y *= proportion.y;
        to.y *= -1.0f;
        vertices[2].m_position = to;
        rendererComponent.m_meshResource.lock()->ReLoad(vertices);

        U32 modelLoc = glGetUniformLocation(rendererComponent.m_shaderResource.lock()->GetProgram(), "model");
        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

        glm::mat4 projection = glm::ortho(0.0f, static_cast<F32>(windowComponent.m_currentResolution.x), -static_cast<F32>(windowComponent.m_currentResolution.y), 0.0f, static_cast<F32>(LayerType::NEAR_PLANE), -static_cast<F32>(LayerType::FAR_PLANE));
        U32 projectionLoc = glGetUniformLocation(rendererComponent.m_shaderResource.lock()->GetProgram(), "projection");
        glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection));

        U32 colorLoc = glGetUniformLocation(rendererComponent.m_shaderResource.lock()->GetProgram(), "color");
        glUniform4fv(colorLoc, 1, glm::value_ptr(weaponComponent.lock()->m_hasHit ? Yellow : color));

        U32 pctLoc = glGetUniformLocation(rendererComponent.m_shaderResource.lock()->GetProgram(), "pct");
        glUniform1f(pctLoc, 1.0f);

        glBindVertexArray(rendererComponent.m_meshResource.lock()->GetVAO());
        glDrawElements(GL_LINES, 2, GL_UNSIGNED_INT, 0);

        glBindTexture(GL_TEXTURE_2D, 0);
        glBindVertexArray(0);
    }

    return true;
}

//----------------------------------------------------------------------------------------------------
void WeaponSystemServer::Rewind(Entity localEntity, U32 from, U32 to, F32 percentage) const
{
    LinkingContext& linkingContext = g_gameServer->GetLinkingContext();
    const std::unordered_map<NetworkID, Entity>& newtorkIDToEntity = linkingContext.GetNetworkIDToEntityMap();

    for (const auto& pair : newtorkIDToEntity) {
        Entity remoteEntity = pair.second;
        if (localEntity == remoteEntity) {
            continue;
        }

        std::weak_ptr<TransformComponent> transformComponent = g_gameServer->GetComponentManager().GetComponent<TransformComponent>(remoteEntity);
        std::weak_ptr<RigidbodyComponent> rigidbodyComponent = g_gameServer->GetComponentManager().GetComponent<RigidbodyComponent>(remoteEntity);
        if (transformComponent.expired() || rigidbodyComponent.expired()) {
            continue;
        }

        if (!transformComponent.lock()->m_transformBuffer.IsEmpty()) {
            Transform fromTransform;
            for (U32 i = transformComponent.lock()->m_transformBuffer.m_front; i < transformComponent.lock()->m_transformBuffer.m_back; ++i) {
                Transform& t = transformComponent.lock()->m_transformBuffer.Get(i);
                if (t.GetFrame() == from) {
                    fromTransform = t;
                    break;
                }
            }

            Transform toTransform;
            for (U32 i = transformComponent.lock()->m_transformBuffer.m_front; i < transformComponent.lock()->m_transformBuffer.m_back; ++i) {
                Transform& t = transformComponent.lock()->m_transformBuffer.Get(i);
                if (t.GetFrame() == to) {
                    toTransform = t;
                    break;
                }
            }

            glm::vec2 interpolatedPosition = Lerp(fromTransform.m_position, toTransform.m_position, percentage);
            F32 interpolatedRotation = Lerp(fromTransform.m_rotation, toTransform.m_rotation, percentage);
            rigidbodyComponent.lock()->m_body->SetTransform(b2Vec2(PIXELS_TO_METERS(interpolatedPosition.x), PIXELS_TO_METERS(interpolatedPosition.y)), glm::radians(interpolatedRotation));
            ILOG("SERVER REWIND POS %f %f ROT %f", interpolatedPosition.x, interpolatedPosition.y, interpolatedRotation);
        }
    }
}

//----------------------------------------------------------------------------------------------------
void WeaponSystemServer::Revert(Entity localEntity) const
{
    LinkingContext& linkingContext = g_gameServer->GetLinkingContext();
    const std::unordered_map<NetworkID, Entity>& newtorkIDToEntity = linkingContext.GetNetworkIDToEntityMap();

    for (const auto& pair : newtorkIDToEntity) {
        Entity remoteEntity = pair.second;
        if (localEntity == remoteEntity) {
            continue;
        }

        std::weak_ptr<TransformComponent> transformComponent = g_gameServer->GetComponentManager().GetComponent<TransformComponent>(remoteEntity);
        std::weak_ptr<RigidbodyComponent> rigidbodyComponent = g_gameServer->GetComponentManager().GetComponent<RigidbodyComponent>(remoteEntity);
        if (transformComponent.expired() || rigidbodyComponent.expired()) {
            continue;
        }

        glm::vec2 position = transformComponent.lock()->m_position;
        F32 rotation = transformComponent.lock()->m_rotation;
        rigidbodyComponent.lock()->m_body->SetTransform(b2Vec2(PIXELS_TO_METERS(position.x), PIXELS_TO_METERS(position.y)), glm::radians(rotation));
    }
}
}
