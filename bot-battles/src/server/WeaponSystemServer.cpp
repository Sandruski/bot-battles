#include "WeaponSystemServer.h"

#include "BotComponent.h"
#include "ClientProxy.h"
#include "ComponentManager.h"
#include "ComponentMemberTypes.h"
#include "GameServer.h"
#include "GameplayComponent.h"
#include "HealthComponent.h"
#include "LinkingContext.h"
#include "MeshResource.h"
#include "PhysicsComponent.h"
#include "RendererComponent.h"
#include "ReplicationManagerServer.h"
#include "RigidbodyComponent.h"
#include "ServerComponent.h"
#include "ShaderResource.h"
#include "SpriteComponent.h"
#include "State.h"
#include "SystemManager.h"
#include "TransformComponent.h"
#include "WeaponComponent.h"

namespace sand {
//----------------------------------------------------------------------------------------------------
WeaponSystemServer::WeaponSystemServer()
{
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

    std::weak_ptr<GameplayComponent> gameplayComponent = g_gameServer->GetGameplayComponent();
    std::weak_ptr<State> currentState = gameplayComponent.lock()->m_fsm.GetCurrentState();
    if (currentState.expired()) {
        return true;
    }

    std::weak_ptr<ServerComponent> serverComponent = g_gameServer->GetServerComponent();
    for (auto& entity : m_entities) {
        PlayerID playerID = serverComponent.lock()->GetPlayerID(entity);
        if (playerID >= INVALID_PLAYER_ID) {
            continue;
        }

        std::weak_ptr<WeaponComponent> weaponComponent = g_gameServer->GetComponentManager().GetComponent<WeaponComponent>(entity);
        std::weak_ptr<TransformComponent> transformComponent = g_gameServer->GetComponentManager().GetComponent<TransformComponent>(entity);
        std::weak_ptr<RigidbodyComponent> rigidbodyComponent = g_gameServer->GetComponentManager().GetComponent<RigidbodyComponent>(entity);
        std::weak_ptr<SpriteComponent> spriteComponent = g_gameServer->GetComponentManager().GetComponent<SpriteComponent>(entity);
        std::weak_ptr<BotComponent> botComponent = g_gameServer->GetComponentManager().GetComponent<BotComponent>(entity);

        U64 characterDirtyState = 0;

        std::weak_ptr<ClientProxy> clientProxy = serverComponent.lock()->GetClientProxy(playerID);
        for (U32 i = clientProxy.lock()->m_inputBuffer.m_front; i < clientProxy.lock()->m_inputBuffer.m_back; ++i) {
            const bool canPerformAction = botComponent.lock()->CanPerformAction();
            if (!canPerformAction) {
                continue;
            }

            const Input& input = clientProxy.lock()->m_inputBuffer.Get(i);
            U64 dirtyState = input.GetDirtyState();

            // Reload
            const bool hasReload = dirtyState & static_cast<U64>(InputComponentMemberType::INPUT_RELOAD);
            if (hasReload) {
                const bool result = weaponComponent.lock()->CanReload();
                if (!result) {
                    continue;
                }

                weaponComponent.lock()->Reload();
                characterDirtyState |= static_cast<U64>(ComponentMemberType::WEAPON_CURRENT_AMMO_PRIMARY);
                characterDirtyState |= static_cast<U64>(ComponentMemberType::WEAPON_AMMO_PRIMARY);

                spriteComponent.lock()->m_spriteName = "reload";
                characterDirtyState |= static_cast<U64>(ComponentMemberType::SPRITE_SPRITE_NAME);

                botComponent.lock()->m_actionType = BotComponent::ActionType::RELOAD;
                characterDirtyState |= static_cast<U64>(ComponentMemberType::BOT_ACTION_TYPE);
                botComponent.lock()->m_timeAction = weaponComponent.lock()->m_timeReload;
                characterDirtyState |= static_cast<U64>(ComponentMemberType::BOT_TIME_ACTION);
                botComponent.lock()->m_cooldownAction = weaponComponent.lock()->m_cooldownReload;
                botComponent.lock()->m_timerAction.Start();
            }

            // Shoot
            const bool hasShootPrimaryWeapon = dirtyState & static_cast<U64>(InputComponentMemberType::INPUT_SHOOT_PRIMARY_WEAPON);
            const bool hasShootSecondaryWeapon = dirtyState & static_cast<U64>(InputComponentMemberType::INPUT_SHOOT_SECONDARY_WEAPON);
            if (hasShootPrimaryWeapon || hasShootSecondaryWeapon) {
                if (hasShootPrimaryWeapon) {
                    const bool result = weaponComponent.lock()->CanShootPrimary();
                    if (!result) {
                        continue;
                    }

                    weaponComponent.lock()->ShootPrimary();
                    characterDirtyState |= static_cast<U64>(ComponentMemberType::WEAPON_CURRENT_AMMO_PRIMARY);

                    spriteComponent.lock()->m_spriteName = "shootPrimary";
                    characterDirtyState |= static_cast<U64>(ComponentMemberType::SPRITE_SPRITE_NAME);

                    botComponent.lock()->m_actionType = BotComponent::ActionType::SHOOT_PRIMARY;
                    characterDirtyState |= static_cast<U64>(ComponentMemberType::BOT_ACTION_TYPE);
                    botComponent.lock()->m_timeAction = weaponComponent.lock()->m_timeShootPrimary;
                    characterDirtyState |= static_cast<U64>(ComponentMemberType::BOT_TIME_ACTION);
                    botComponent.lock()->m_cooldownAction = weaponComponent.lock()->m_cooldownShootPrimary;
                    botComponent.lock()->m_timerAction.Start();
                } else {
                    const bool result = weaponComponent.lock()->CanShootSecondary();
                    if (!result) {
                        continue;
                    }

                    weaponComponent.lock()->ShootSecondary();

                    spriteComponent.lock()->m_spriteName = "shootSecondary";
                    characterDirtyState |= static_cast<U64>(ComponentMemberType::SPRITE_SPRITE_NAME);

                    botComponent.lock()->m_actionType = BotComponent::ActionType::SHOOT_SECONDARY;
                    characterDirtyState |= static_cast<U64>(ComponentMemberType::BOT_ACTION_TYPE);
                    botComponent.lock()->m_timeAction = weaponComponent.lock()->m_timeShootSecondary;
                    characterDirtyState |= static_cast<U64>(ComponentMemberType::BOT_TIME_ACTION);
                    botComponent.lock()->m_cooldownAction = weaponComponent.lock()->m_cooldownShootSecondary;
                    botComponent.lock()->m_timerAction.Start();
                }

                if (serverComponent.lock()->m_isServerRewind) {
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

                weaponComponent.lock()->m_originLastShot = position;
                characterDirtyState |= static_cast<U64>(ComponentMemberType::WEAPON_ORIGIN_LAST_SHOT);
                F32 maxLength = hasShootPrimaryWeapon ? weaponComponent.lock()->m_rangePrimary : weaponComponent.lock()->m_rangeSecondary;
                weaponComponent.lock()->m_destinationLastShot = position + rotation * maxLength;
                characterDirtyState |= static_cast<U64>(ComponentMemberType::WEAPON_DESTINATION_LAST_SHOT);
                weaponComponent.lock()->m_alpha = 1.0f;

                weaponComponent.lock()->m_hitEntityLastShot = INVALID_ENTITY;
                characterDirtyState |= static_cast<U64>(ComponentMemberType::WEAPON_HIT_ENTITY_LAST_SHOT);

                std::weak_ptr<PhysicsComponent> physicsComponent = g_gameServer->GetPhysicsComponent();
                PhysicsComponent::RaycastHit hitInfo;
                const bool hasIntersected = physicsComponent.lock()->Raycast(weaponComponent.lock()->m_originLastShot, weaponComponent.lock()->m_destinationLastShot, INVALID_ENTITY, hitInfo);
                if (hasIntersected) {
                    std::weak_ptr<TransformComponent> hitEntityTransformComponent = g_gameServer->GetComponentManager().GetComponent<TransformComponent>(hitInfo.m_entity);
                    if (!hitEntityTransformComponent.expired()) {
                        weaponComponent.lock()->m_destinationLastShot = hitInfo.m_point;
                    }

                    if (hitInfo.m_entity != entity) {
                        std::weak_ptr<HealthComponent> hitEntityHealthComponent = g_gameServer->GetComponentManager().GetComponent<HealthComponent>(hitInfo.m_entity);
                        if (!hitEntityHealthComponent.expired()) {
                            weaponComponent.lock()->m_hitEntityLastShot = hitInfo.m_entity;

                            Event newWeaponEvent;
                            newWeaponEvent.eventType = EventType::WEAPON_HIT;
                            newWeaponEvent.weapon.shooterEntity = entity;
                            newWeaponEvent.weapon.targetEntity = hitInfo.m_entity;
                            newWeaponEvent.weapon.damage = hasShootPrimaryWeapon ? weaponComponent.lock()->m_damagePrimary : weaponComponent.lock()->m_damageSecondary;
                            newWeaponEvent.weapon.direction = glm::normalize(weaponComponent.lock()->m_destinationLastShot - weaponComponent.lock()->m_originLastShot);
                            PushEvent(newWeaponEvent);
                        }
                    }
                }

                rigidbodyComponent.lock()->m_body->SetTransform(b2Vec2(PIXELS_TO_METERS(transformComponent.lock()->m_position.x), PIXELS_TO_METERS(transformComponent.lock()->m_position.y)), glm::radians(transformComponent.lock()->m_rotation));

                if (serverComponent.lock()->m_isServerRewind) {
                    Revert(entity);
                }
            }
        }

        if (characterDirtyState > 0) {
            Event newEvent;
            newEvent.eventType = EventType::COMPONENT_MEMBER_CHANGED;
            newEvent.component.entity = entity;
            newEvent.component.dirtyState = characterDirtyState;
            NotifyEvent(newEvent);
        }
    }

    return true;
}

//----------------------------------------------------------------------------------------------------
bool WeaponSystemServer::Render()
{
    OPTICK_EVENT();

    F32 dt = MyTime::GetInstance().GetDt();

    std::weak_ptr<ServerComponent> serverComponent = g_gameServer->GetServerComponent();
    for (const auto& entity : m_entities) {
        PlayerID playerID = serverComponent.lock()->GetPlayerID(entity);
        if (playerID >= INVALID_PLAYER_ID) {
            continue;
        }

        std::weak_ptr<BotComponent> botComponent = g_gameServer->GetComponentManager().GetComponent<BotComponent>(entity);
        if (botComponent.lock()->m_actionType != BotComponent::ActionType::SHOOT_PRIMARY
            && botComponent.lock()->m_actionType != BotComponent::ActionType::SHOOT_SECONDARY) {
            continue;
        }

        std::weak_ptr<WeaponComponent> weaponComponent = g_gameServer->GetComponentManager().GetComponent<WeaponComponent>(entity);
        weaponComponent.lock()->m_alpha -= dt / botComponent.lock()->m_timeAction;
        if (weaponComponent.lock()->m_alpha <= 0.0f) {
            weaponComponent.lock()->m_alpha = 0.0f;
        }
        Draw(playerID, weaponComponent);
    }

    return true;
}

//----------------------------------------------------------------------------------------------------
bool WeaponSystemServer::RenderGui()
{
    OPTICK_EVENT();

    std::weak_ptr<ServerComponent> serverComponent = g_gameServer->GetServerComponent();
    for (const auto& entity : m_entities) {
        PlayerID playerID = serverComponent.lock()->GetPlayerID(entity);
        if (playerID >= INVALID_PLAYER_ID) {
            continue;
        }

        std::weak_ptr<WeaponComponent> weaponComponent = g_gameServer->GetComponentManager().GetComponent<WeaponComponent>(entity);
        DrawGui(playerID, weaponComponent);
    }

    return true;
}

//----------------------------------------------------------------------------------------------------
void WeaponSystemServer::Rewind(Entity localEntity, U32 from, U32 to, F32 percentage) const
{
    std::weak_ptr<ServerComponent> serverComponent = g_gameServer->GetServerComponent();
    LinkingContext& linkingContext = g_gameServer->GetLinkingContext();
    const std::unordered_map<NetworkID, Entity>& newtorkIDToEntity = linkingContext.GetNetworkIDToEntityMap();
    for (const auto& pair : newtorkIDToEntity) {
        Entity remoteEntity = pair.second;
        if (localEntity == remoteEntity) {
            continue;
        }

        PlayerID playerID = serverComponent.lock()->GetPlayerID(remoteEntity);
        if (playerID >= INVALID_PLAYER_ID) {
            continue;
        }

        std::weak_ptr<TransformComponent> transformComponent = g_gameServer->GetComponentManager().GetComponent<TransformComponent>(remoteEntity);
        std::weak_ptr<RigidbodyComponent> rigidbodyComponent = g_gameServer->GetComponentManager().GetComponent<RigidbodyComponent>(remoteEntity);
        if (transformComponent.expired() || rigidbodyComponent.expired()) {
            continue;
        }

        if (!transformComponent.lock()->m_transformBuffer.IsEmpty()) {
            Transform fromTransform;
            bool isFound = false;
            for (U32 i = transformComponent.lock()->m_transformBuffer.m_front; i < transformComponent.lock()->m_transformBuffer.m_back; ++i) {
                Transform& t = transformComponent.lock()->m_transformBuffer.Get(i);
                if (t.GetFrame() == from) {
                    fromTransform = t;
                    isFound = true;
                    break;
                }
            }

            if (!isFound) {
                return;
            }

            Transform toTransform;
            isFound = false;
            for (U32 i = transformComponent.lock()->m_transformBuffer.m_front; i < transformComponent.lock()->m_transformBuffer.m_back; ++i) {
                Transform& t = transformComponent.lock()->m_transformBuffer.Get(i);
                if (t.GetFrame() == to) {
                    toTransform = t;
                    isFound = true;
                    break;
                }
            }

            if (!isFound) {
                return;
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
    std::weak_ptr<ServerComponent> serverComponent = g_gameServer->GetServerComponent();
    LinkingContext& linkingContext = g_gameServer->GetLinkingContext();
    const std::unordered_map<NetworkID, Entity>& newtorkIDToEntity = linkingContext.GetNetworkIDToEntityMap();
    for (const auto& pair : newtorkIDToEntity) {
        Entity remoteEntity = pair.second;
        if (localEntity == remoteEntity) {
            continue;
        }

        PlayerID playerID = serverComponent.lock()->GetPlayerID(remoteEntity);
        if (playerID >= INVALID_PLAYER_ID) {
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
