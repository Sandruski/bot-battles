#include "WeaponSystemServer.h"

#include "ClientProxy.h"
#include "ColliderComponent.h"
#include "ComponentManager.h"
#include "ComponentMemberTypes.h"
#include "DebugDrawer.h"
#include "GameServer.h"
#include "HealthComponent.h"
#include "Intersection.h"
#include "LinkingContext.h"
#include "ServerComponent.h"
#include "SystemManager.h"
#include "TransformComponent.h"
#include "WeaponComponent.h"
#include "WindowComponent.h"

namespace sand {
//----------------------------------------------------------------------------------------------------
WeaponSystemServer::WeaponSystemServer()
{
    m_signature |= 1 << static_cast<U16>(ComponentType::TRANSFORM);
    m_signature |= 1 << static_cast<U16>(ComponentType::WEAPON);
}

//----------------------------------------------------------------------------------------------------
bool WeaponSystemServer::Update()
{
    GameplayComponent& gameplayComponent = g_gameServer->GetGameplayComponent();
    if (gameplayComponent.m_phaseType != PhaseType::PLAY) {
        return true;
    }

    ServerComponent serverComponent = g_gameServer->GetServerComponent();

    for (auto& entity : m_entities) {
        PlayerID playerID = serverComponent.GetPlayerID(entity);
        if (playerID >= INVALID_PLAYER_ID) {
            continue;
        }

        std::weak_ptr<ClientProxy> clientProxy = serverComponent.GetClientProxy(playerID);
        std::weak_ptr<TransformComponent> transformComponent = g_gameServer->GetComponentManager().GetComponent<TransformComponent>(entity);
        std::weak_ptr<WeaponComponent> weaponComponent = g_gameServer->GetComponentManager().GetComponent<WeaponComponent>(entity);

        for (U32 i = clientProxy.lock()->m_inputBuffer.m_front; i < clientProxy.lock()->m_inputBuffer.m_back; ++i) {
            const Input& input = clientProxy.lock()->m_inputBuffer.Get(i);
            const InputComponent& inputComponent = input.GetInputComponent();
            if (inputComponent.m_isShooting) {
                ILOG("Frame %u of playerID %u is shooting", i, playerID);
                if (serverComponent.m_isServerRewind) {
                    Rewind(entity, input.m_interpolationFromFrame, input.m_interpolationToFrame, input.m_interpolationPercentage);
                }

                Transform transform;
                bool has = false;
                for (U32 j = transformComponent.lock()->m_inputTransformBuffer.m_front; j < transformComponent.lock()->m_inputTransformBuffer.m_back; ++j) {
                    Transform& t = transformComponent.lock()->m_inputTransformBuffer.Get(j);
                    if (t.GetFrame() == input.GetFrame()) {
                        transform = t;
                        has = true;
                        break;
                    }
                }
                if (!has) {
                    transform = transformComponent.lock()->m_inputTransformBuffer.GetLast();
                }

                glm::vec2 position = { transform.m_position.x, transform.m_position.y };
                ILOG("SERVER Pos is: %f %f", position.x, position.y);
                ILOG("From frame %u to frame %u percentage %f", input.m_interpolationFromFrame, input.m_interpolationToFrame, input.m_interpolationPercentage);

                F32 x = std::cos(glm::radians(transform.m_rotation));
                F32 y = std::sin(glm::radians(transform.m_rotation));
                glm::vec2 rotation = { x, y };
                weaponComponent.lock()->m_origin = position;
                WindowComponent& windowComponent = g_gameServer->GetWindowComponent();
                F32 maxLength = static_cast<F32>(std::max(windowComponent.m_resolution.x, windowComponent.m_resolution.y));
                weaponComponent.lock()->m_destination = position + rotation * maxLength;
                std::pair<Entity, std::weak_ptr<ColliderComponent>> intersection;
                const bool hasIntersected = Raycast(position, rotation, maxLength, intersection);
                if (hasIntersected) {
                    weaponComponent.lock()->m_hasHit = true;
                    Entity hitEntity = intersection.first;
                    std::weak_ptr<HealthComponent> healthComponent = g_gameServer->GetComponentManager().GetComponent<HealthComponent>(hitEntity);
                    healthComponent.lock()->m_health -= 100;
                    ILOG("Health is %u", healthComponent.lock()->m_health);

                    Event newEvent;
                    newEvent.eventType = EventType::COMPONENT_MEMBER_CHANGED;
                    newEvent.component.entity = entity;
                    newEvent.component.dirtyState = static_cast<U32>(ComponentMemberType::HEALTH_HEALTH);
                    NotifyEvent(newEvent);
                } else {
                    weaponComponent.lock()->m_hasHit = false;
                }

                if (serverComponent.m_isServerRewind) {
                    Revert(entity);
                }
            }
        }
    }

    return true;
}

//----------------------------------------------------------------------------------------------------
bool WeaponSystemServer::DebugRender()
{
    ServerComponent& serverComponent = g_gameServer->GetServerComponent();

    for (auto& entity : m_entities) {
        PlayerID playerID = serverComponent.GetPlayerID(entity);
        if (playerID >= INVALID_PLAYER_ID) {
            continue;
        }

        //std::weak_ptr<WeaponComponent> weaponComponent = g_gameServer->GetComponentManager().GetComponent<WeaponComponent>(entity);
        //DebugDrawer::DrawLine(weaponComponent.lock()->GetShotRect(), weaponComponent.lock()->m_hasHit ? Red : Black);
    }

    return true;
}

//----------------------------------------------------------------------------------------------------
void WeaponSystemServer::Rewind(Entity localEntity, U32 from, U32 to, F32 percentage)
{
    LinkingContext& linkingContext = g_gameServer->GetLinkingContext();
    const std::unordered_map<NetworkID, Entity>& newtorkIDToEntity = linkingContext.GetNetworkIDToEntityMap();

    for (const auto& pair : newtorkIDToEntity) {
        Entity remoteEntity = pair.second;
        if (localEntity == remoteEntity) {
            continue;
        }

        std::weak_ptr<TransformComponent> transformComponent = g_gameServer->GetComponentManager().GetComponent<TransformComponent>(remoteEntity);
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

            glm::vec3 position = Lerp(fromTransform.m_position, toTransform.m_position, percentage);
            std::weak_ptr<ColliderComponent> colliderComponent = g_gameServer->GetComponentManager().GetComponent<ColliderComponent>(remoteEntity);
            colliderComponent.lock()->m_position = { position.x, position.y };
            colliderComponent.lock()->m_shotPosition = colliderComponent.lock()->m_position;
            ILOG("SERVER Rewind is %f %f", colliderComponent.lock()->m_shotPosition.x, colliderComponent.lock()->m_shotPosition.y);
            ILOG("From trans %u position %f %f, to trans %u position %f %f", from, fromTransform.m_position.x, fromTransform.m_position.y,
                to, toTransform.m_position.x, toTransform.m_position.y);
        }
    }
}

//----------------------------------------------------------------------------------------------------
void WeaponSystemServer::Revert(Entity localEntity)
{
    LinkingContext& linkingContext = g_gameServer->GetLinkingContext();
    const std::unordered_map<NetworkID, Entity>& newtorkIDToEntity = linkingContext.GetNetworkIDToEntityMap();

    for (const auto& pair : newtorkIDToEntity) {
        Entity remoteEntity = pair.second;
        if (localEntity == remoteEntity) {
            continue;
        }

        std::weak_ptr<TransformComponent> transformComponent = g_gameServer->GetComponentManager().GetComponent<TransformComponent>(remoteEntity);
        std::weak_ptr<ColliderComponent> colliderComponent = g_gameServer->GetComponentManager().GetComponent<ColliderComponent>(remoteEntity);
        colliderComponent.lock()->m_position = transformComponent.lock()->GetPosition();
    }
}
}
