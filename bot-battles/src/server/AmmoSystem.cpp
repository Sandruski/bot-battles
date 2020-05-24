#include "AmmoSystem.h"

#include "AmmoSpawnerComponent.h"
#include "ClientProxy.h"
#include "ColliderComponent.h"
#include "ComponentManager.h"
#include "ComponentMemberTypes.h"
#include "EntityManager.h"
#include "GameServer.h"
#include "GameplayComponent.h"
#include "LinkingContext.h"
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
    //LinkingContext& linkingContext = g_gameServer->GetLinkingContext();
    PhysicsComponent& physicsComponent = g_gameServer->GetPhysicsComponent();
    for (auto& entity : m_entities) {
        PlayerID playerID = serverComponent.GetPlayerID(entity);
        if (playerID >= INVALID_PLAYER_ID) {
            continue;
        }

        std::weak_ptr<TransformComponent> transformComponent = g_gameServer->GetComponentManager().GetComponent<TransformComponent>(entity);
        std::weak_ptr<ColliderComponent> colliderComponent = g_gameServer->GetComponentManager().GetComponent<ColliderComponent>(entity);
        std::weak_ptr<WeaponComponent> weaponComponent = g_gameServer->GetComponentManager().GetComponent<WeaponComponent>(entity);
        if (!transformComponent.lock()->m_isEnabled || !weaponComponent.lock()->m_isEnabled) {
            continue;
        }

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
                        /*
                        NetworkID networkID = linkingContext.GetNetworkID(overlapEntity);
                        if (networkID >= INVALID_NETWORK_ID) {
                            continue;
                        }
                        */

                        std::weak_ptr<AmmoSpawnerComponent> ammoSpawnerComponent = g_gameServer->GetComponentManager().GetComponent<AmmoSpawnerComponent>(overlapEntity);
                        if (ammoSpawnerComponent.expired() || !ammoSpawnerComponent.lock()->m_isEnabled) {
                            continue;
                        }

                        U32 ammo = ammoSpawnerComponent.lock()->PickUp();
                        weaponComponent.lock()->Reload(ammo);
                        g_gameServer->GetEntityManager().RemoveEntity(overlapEntity);
                    }
                }
            }
        }
    }

    return true;
}
}
