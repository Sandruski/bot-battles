#include "WeaponSystemServer.h"

#include "ClientProxy.h"
#include "ComponentManager.h"
#include "ComponentMemberTypes.h"
#include "GameServer.h"
#include "HealthComponent.h"
#include "LinkingContext.h"
#include "MeshResource.h"
#include "PhysicsComponent.h"
#include "RendererComponent.h"
#include "ReplicationManagerServer.h"
#include "RigidbodyComponent.h"
#include "ServerComponent.h"
#include "ShaderResource.h"
#include "State.h"
#include "SystemManager.h"
#include "TransformComponent.h"
#include "WeaponComponent.h"

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
    OPTICK_EVENT();

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

        std::weak_ptr<ClientProxy> clientProxy = serverComponent.GetClientProxy(playerID);
        if (!clientProxy.lock()->m_inputBuffer.IsEmpty()) {
            std::weak_ptr<TransformComponent> transformComponent = g_gameServer->GetComponentManager().GetComponent<TransformComponent>(entity);
            std::weak_ptr<RigidbodyComponent> rigidbodyComponent = g_gameServer->GetComponentManager().GetComponent<RigidbodyComponent>(entity);
            std::weak_ptr<WeaponComponent> weaponComponent = g_gameServer->GetComponentManager().GetComponent<WeaponComponent>(entity);

            U64 weaponDirtyState = 0;
            for (U32 i = clientProxy.lock()->m_inputBuffer.m_front; i < clientProxy.lock()->m_inputBuffer.m_back; ++i) {
                const Input& input = clientProxy.lock()->m_inputBuffer.Get(i);
                U64 dirtyState = input.GetDirtyState();

                const bool hasShootPrimaryWeapon = dirtyState & static_cast<U64>(InputComponentMemberType::INPUT_SHOOT_PRIMARY_WEAPON);
                const bool hasShootSecondaryWeapon = dirtyState & static_cast<U64>(InputComponentMemberType::INPUT_SHOOT_SECONDARY_WEAPON);
                if (hasShootPrimaryWeapon || hasShootSecondaryWeapon) {
                    F32 timestamp = input.GetTimestamp();

                    if (hasShootPrimaryWeapon) {
                        F32 timestampDiff = timestamp - weaponComponent.lock()->m_timestampLastShotPrimary;
                        if (timestampDiff < weaponComponent.lock()->m_cooldownPrimary) {
                            continue;
                        }
                        if (weaponComponent.lock()->m_ammoPrimary == 0) {
                            continue;
                        }
                        weaponComponent.lock()->m_ammoPrimary -= 1;
                        weaponDirtyState |= static_cast<U64>(ComponentMemberType::WEAPON_AMMO_PRIMARY);
                    } else if (hasShootSecondaryWeapon) {
                        F32 timestampDiff = timestamp - weaponComponent.lock()->m_timestampLastShotSecondary;
                        if (timestampDiff < weaponComponent.lock()->m_cooldownSecondary) {
                            continue;
                        }
                    }
                    weaponComponent.lock()->m_timestampLastShotSecondary = timestamp;
                    weaponComponent.lock()->m_timerShot.Start();

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

                    glm::vec2 position = transform.m_position;
                    F32 x = std::cos(glm::radians(transform.m_rotation));
                    F32 y = std::sin(glm::radians(transform.m_rotation));
                    glm::vec2 rotation = glm::vec2(x, y);

                    weaponComponent.lock()->m_originShot = position;
                    F32 maxLength = hasShootPrimaryWeapon ? weaponComponent.lock()->m_rangePrimary : weaponComponent.lock()->m_rangeSecondary;
                    weaponComponent.lock()->m_destinationShot = position + rotation * maxLength;

                    PhysicsComponent& physicsComponent = g_gameServer->GetPhysicsComponent();
                    PhysicsComponent::RaycastHit hitInfo;
                    const bool hasIntersected = physicsComponent.Raycast(weaponComponent.lock()->m_originShot, weaponComponent.lock()->m_destinationShot, hitInfo);
                    if (hasIntersected) {
                        std::weak_ptr<TransformComponent> hitEntityTransformComponent = g_gameServer->GetComponentManager().GetComponent<TransformComponent>(hitInfo.m_entity);
                        if (!hitEntityTransformComponent.expired()) {
                            weaponComponent.lock()->m_destinationShot = hitInfo.m_point;
                        }

                        if (hitInfo.m_entity != entity) {
                            std::weak_ptr<HealthComponent> hitEntityHealthComponent = g_gameServer->GetComponentManager().GetComponent<HealthComponent>(hitInfo.m_entity);
                            if (!hitEntityHealthComponent.expired()) {
                                Event newEvent;
                                newEvent.eventType = EventType::WEAPON_HIT;
                                newEvent.weapon.entity = entity;
                                newEvent.weapon.damage = hasShootPrimaryWeapon ? weaponComponent.lock()->m_damagePrimary : weaponComponent.lock()->m_damageSecondary;
                                PushEvent(newEvent);
                            }
                        }
                    }

                    rigidbodyComponent.lock()->m_body->SetTransform(b2Vec2(PIXELS_TO_METERS(transformComponent.lock()->m_position.x), PIXELS_TO_METERS(transformComponent.lock()->m_position.y)), glm::radians(transformComponent.lock()->m_rotation));

                    if (serverComponent.m_isServerRewind) {
                        Revert(entity);
                    }
                }
            }

            if (weaponDirtyState > 0) {
                Event newEvent;
                newEvent.eventType = EventType::COMPONENT_MEMBER_CHANGED;
                newEvent.component.entity = entity;
                newEvent.component.dirtyState = weaponDirtyState;
                NotifyEvent(newEvent);
            }
        }
    }

    return true;
}

//----------------------------------------------------------------------------------------------------
bool WeaponSystemServer::Render()
{
    OPTICK_EVENT();

    RendererComponent& rendererComponent = g_gameServer->GetRendererComponent();
    ServerComponent& serverComponent = g_gameServer->GetServerComponent();
    for (const auto& entity : m_entities) {
        PlayerID playerID = serverComponent.GetPlayerID(entity);
        if (playerID >= INVALID_PLAYER_ID) {
            continue;
        }

        std::weak_ptr<WeaponComponent> weaponComponent = g_gameServer->GetComponentManager().GetComponent<WeaponComponent>(entity);

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
        Draw(rendererComponent, weaponComponent, color);
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
