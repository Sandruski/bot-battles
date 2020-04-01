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
    m_signature |= 1 << static_cast<U16>(ComponentType::WEAPON);
}

//----------------------------------------------------------------------------------------------------
bool WeaponSystemServer::Update()
{
    GameplayComponent& gameplayComponent = g_gameServer->GetGameplayComponent();
    std::weak_ptr<State> currentState = gameplayComponent.m_fsm.GetCurrentState();
    if (currentState.expired() || currentState.lock()->GetName() != "Play") {
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
                weaponComponent.lock()->m_positions.clear();

                if (serverComponent.m_isServerRewind) {
                    Rewind(weaponComponent, entity, input.m_interpolationFromFrame, input.m_interpolationToFrame, input.m_interpolationPercentage);
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
                glm::vec2 position = { transform.m_position.x, transform.m_position.y };
                weaponComponent.lock()->m_origin = position;
                F32 x = std::cos(glm::radians(transform.m_rotation));
                F32 y = std::sin(glm::radians(transform.m_rotation));
                glm::vec2 rotation = glm::vec2(x, y);
                WindowComponent& windowComponent = g_gameServer->GetWindowComponent();
                F32 maxLength = static_cast<F32>(std::max(windowComponent.m_resolution.x, windowComponent.m_resolution.y));
                std::pair<Entity, std::weak_ptr<ColliderComponent>> object;
                glm::vec2 intersection;
                const bool hasIntersected = Raycast(position, rotation, maxLength, object, intersection);
                if (hasIntersected) {
                    Entity hitEntity = object.first;

                    std::weak_ptr<TransformComponent> hitEntityTransformComponent = g_gameServer->GetComponentManager().GetComponent<TransformComponent>(hitEntity);
                    if (!hitEntityTransformComponent.expired()) {
                        weaponComponent.lock()->m_destination = intersection;
                    }

                    std::weak_ptr<HealthComponent> hitEntityHealthComponent = g_gameServer->GetComponentManager().GetComponent<HealthComponent>(hitEntity);
                    if (!hitEntityHealthComponent.expired()) {
                        weaponComponent.lock()->m_hasHit = true;

                        hitEntityHealthComponent.lock()->m_health -= 100;
                        ILOG("Health is %u", hitEntityHealthComponent.lock()->m_health);

                        Event newEvent;
                        newEvent.eventType = EventType::COMPONENT_MEMBER_CHANGED;
                        newEvent.component.entity = entity;
                        newEvent.component.dirtyState = static_cast<U32>(ComponentMemberType::HEALTH_HEALTH);
                        NotifyEvent(newEvent);
                    } else {
                        weaponComponent.lock()->m_hasHit = false;
                    }
                } else {
                    weaponComponent.lock()->m_destination = position + rotation * maxLength;
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
    return true;
}

//----------------------------------------------------------------------------------------------------
void WeaponSystemServer::Rewind(std::weak_ptr<WeaponComponent> weaponComponent, Entity localEntity, U32 from, U32 to, F32 percentage)
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
        if (transformComponent.expired() || colliderComponent.expired()) {
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

            glm::vec3 position = Lerp(fromTransform.m_position, toTransform.m_position, percentage);
            colliderComponent.lock()->m_position = glm::vec2(position.x, position.y);
        }

        weaponComponent.lock()->m_positions.emplace_back(colliderComponent.lock()->m_position);
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
        if (transformComponent.expired() || colliderComponent.expired()) {
            continue;
        }

        colliderComponent.lock()->m_position = transformComponent.lock()->GetPosition();
    }
}
}
