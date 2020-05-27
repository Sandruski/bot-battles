#include "AmmoSystem.h"

#include "AmmoSpawnerComponent.h"
#include "ClientProxy.h"
#include "ColliderComponent.h"
#include "ComponentManager.h"
#include "ComponentMemberTypes.h"
#include "GameServer.h"
#include "GameplayComponent.h"
#include "LinkingContext.h"
#include "SpriteComponent.h"
#include "State.h"
#include "TransformComponent.h"
#include "WeaponComponent.h"

namespace sand {

//----------------------------------------------------------------------------------------------------
AmmoSystem::AmmoSystem()
{
    m_signature |= 1 << static_cast<U16>(ComponentType::TRANSFORM);
    m_signature |= 1 << static_cast<U16>(ComponentType::COLLIDER);
    m_signature |= 1 << static_cast<U16>(ComponentType::WEAPON);
    m_signature |= 1 << static_cast<U16>(ComponentType::PLAYER);
}

//----------------------------------------------------------------------------------------------------
bool AmmoSystem::Update()
{
    OPTICK_EVENT();

    GameplayComponent& gameplayComponent = g_gameServer->GetGameplayComponent();
    std::weak_ptr<State> currentState = gameplayComponent.m_fsm.GetCurrentState();
    if (currentState.expired()) {
        return true;
    }

    ServerComponent& serverComponent = g_gameServer->GetServerComponent();
    LinkingContext& linkingContext = g_gameServer->GetLinkingContext();
    PhysicsComponent& physicsComponent = g_gameServer->GetPhysicsComponent();
    for (auto& entity : m_entities) {
        PlayerID playerID = serverComponent.GetPlayerID(entity);
        if (playerID >= INVALID_PLAYER_ID) {
            continue;
        }

        std::weak_ptr<TransformComponent> transformComponent = g_gameServer->GetComponentManager().GetComponent<TransformComponent>(entity);
        std::weak_ptr<ColliderComponent> colliderComponent = g_gameServer->GetComponentManager().GetComponent<ColliderComponent>(entity);
        std::weak_ptr<WeaponComponent> weaponComponent = g_gameServer->GetComponentManager().GetComponent<WeaponComponent>(entity);

        std::weak_ptr<ClientProxy> clientProxy = serverComponent.GetClientProxy(playerID);
        for (U32 i = clientProxy.lock()->m_inputBuffer.m_front; i < clientProxy.lock()->m_inputBuffer.m_back; ++i) {
            const Input& input = clientProxy.lock()->m_inputBuffer.Get(i);
            U32 dirtyState = input.GetDirtyState();

            const bool hasPickUpAmmo = dirtyState & static_cast<U32>(InputComponentMemberType::INPUT_PICK_UP_AMMO);
            if (hasPickUpAmmo) {
                glm::vec2 center = transformComponent.lock()->m_position;
                glm::vec2 extents = colliderComponent.lock()->m_size / 2.0f;
                std::vector<Entity> overlapEntities;
                const bool isOverlap = physicsComponent.Overlap(center, extents, overlapEntities);
                if (isOverlap) {
                    for (const auto& overlapEntity : overlapEntities) {
                        NetworkID networkID = linkingContext.GetNetworkID(overlapEntity);
                        if (networkID >= INVALID_NETWORK_ID) {
                            continue;
                        }

                        std::weak_ptr<AmmoSpawnerComponent> ammoSpawnerComponent = g_gameServer->GetComponentManager().GetComponent<AmmoSpawnerComponent>(overlapEntity);
                        std::weak_ptr<SpriteComponent> spriteComponent = g_gameServer->GetComponentManager().GetComponent<SpriteComponent>(overlapEntity);
                        if (ammoSpawnerComponent.expired() || spriteComponent.expired()) {
                            continue;
                        }

                        U32 ammo = ammoSpawnerComponent.lock()->PickUp();
                        if (ammo == 0) {
                            continue;
                        }

                        weaponComponent.lock()->Reload(ammo);
                        spriteComponent.lock()->m_isVisible = false;

                        Event newComponentEvent;
                        newComponentEvent.eventType = EventType::COMPONENT_MEMBER_CHANGED;
                        newComponentEvent.component.dirtyState = static_cast<U32>(ComponentMemberType::AMMO_SPAWNER_AMMO) | static_cast<U32>(ComponentMemberType::SPRITE_VISIBLE);
                        newComponentEvent.component.entity = overlapEntity;
                        NotifyEvent(newComponentEvent);
                        newComponentEvent.component.dirtyState = static_cast<U32>(ComponentMemberType::WEAPON_AMMO);
                        newComponentEvent.component.entity = entity;
                        NotifyEvent(newComponentEvent);
                    }
                }
            }
        }
    }

    return true;
}
}
