#include "WeaponSystemClient.h"

#include "ClientComponent.h"
#include "ColliderComponent.h"
#include "ComponentManager.h"
#include "ComponentMemberTypes.h"
#include "GameClient.h"
#include "HealthComponent.h"
#include "LinkingContext.h"
#include "MeshResource.h"
#include "PhysicsComponent.h"
#include "RendererComponent.h"
#include "ShaderResource.h"
#include "SpriteComponent.h"
#include "State.h"
#include "SystemManager.h"
#include "TransformComponent.h"
#include "WeaponComponent.h"
#include "WindowComponent.h"

namespace sand {

//----------------------------------------------------------------------------------------------------
WeaponSystemClient::WeaponSystemClient()
{
    m_signature |= 1 << static_cast<U16>(ComponentType::WEAPON);
    m_signature |= 1 << static_cast<U16>(ComponentType::PLAYER);
    m_signature |= 1 << static_cast<U16>(ComponentType::LOCAL_PLAYER);
}

//----------------------------------------------------------------------------------------------------
bool WeaponSystemClient::Update()
{
    OPTICK_EVENT();

    GameplayComponent& gameplayComponent = g_gameClient->GetGameplayComponent();
    std::weak_ptr<State> currentState = gameplayComponent.m_fsm.GetCurrentState();
    if (currentState.expired()) {
        return true;
    }

    ClientComponent& clientComponent = g_gameClient->GetClientComponent();
    if (!clientComponent.m_isClientPrediction) {
        return true;
    }

    for (auto& entity : m_entities) {
        if (g_gameClient->GetLinkingContext().GetNetworkID(entity) >= INVALID_NETWORK_ID) {
            continue;
        }

        if (!clientComponent.IsLocalEntity(entity)) {
            continue;
        }

        if (clientComponent.m_isLastShootInputPending) {
            const Input& input = clientComponent.m_inputBuffer.GetLast();
            U64 dirtyState = input.GetDirtyState();

            const bool hasShootPrimaryWeapon = dirtyState & static_cast<U64>(InputComponentMemberType::INPUT_SHOOT_PRIMARY_WEAPON);
            const bool hasShootSecondaryWeapon = dirtyState & static_cast<U64>(InputComponentMemberType::INPUT_SHOOT_SECONDARY_WEAPON);
            if (hasShootPrimaryWeapon || hasShootSecondaryWeapon) {
                std::weak_ptr<WeaponComponent> weaponComponent = g_gameClient->GetComponentManager().GetComponent<WeaponComponent>(entity);
                std::weak_ptr<TransformComponent> transformComponent = g_gameClient->GetComponentManager().GetComponent<TransformComponent>(entity);
                std::weak_ptr<SpriteComponent> spriteComponent = g_gameClient->GetComponentManager().GetComponent<SpriteComponent>(entity);

                F32 timestamp = input.GetTimestamp();

                F32 timestampDiff = timestamp - weaponComponent.lock()->m_timestampShot;
                F32 cooldown = weaponComponent.lock()->m_weaponShot == 1 ? weaponComponent.lock()->m_cooldownPrimary : weaponComponent.lock()->m_cooldownSecondary;
                if (timestampDiff < cooldown) {
                    continue;
                }

                if (hasShootPrimaryWeapon) {
                    if (weaponComponent.lock()->m_ammoPrimary == 0) {
                        continue;
                    }

                    switch (weaponComponent.lock()->m_weaponPrimary) {
                    case 1: {
                        spriteComponent.lock()->m_spriteName = "shootPrimary1";
                        break;
                    }
                    case 2: {
                        spriteComponent.lock()->m_spriteName = "shootPrimary2";
                        break;
                    }
                    default: {
                        break;
                    }
                    }
                } else {
                    spriteComponent.lock()->m_spriteName = "shootSecondary";
                }

                weaponComponent.lock()->m_weaponShot = hasShootPrimaryWeapon ? 1 : 0;
                weaponComponent.lock()->m_timestampShot = timestamp;

                glm::vec2 position = transformComponent.lock()->m_position;
                glm::vec2 rotation = transformComponent.lock()->GetDirection();
                weaponComponent.lock()->m_originShot = position;
                F32 maxLength = hasShootPrimaryWeapon ? weaponComponent.lock()->m_rangePrimary : weaponComponent.lock()->m_rangeSecondary;
                weaponComponent.lock()->m_destinationShot = position + rotation * maxLength;

                PhysicsComponent& physicsComponent = g_gameClient->GetPhysicsComponent();
                PhysicsComponent::RaycastHit hitInfo;
                const bool hasIntersected = physicsComponent.Raycast(weaponComponent.lock()->m_originShot, weaponComponent.lock()->m_destinationShot, hitInfo);
                if (hasIntersected) {
                    std::weak_ptr<TransformComponent> hitEntityTransformComponent = g_gameClient->GetComponentManager().GetComponent<TransformComponent>(hitInfo.m_entity);
                    if (!hitEntityTransformComponent.expired()) {
                        weaponComponent.lock()->m_destinationShot = hitInfo.m_point;
                    }

                    std::weak_ptr<HealthComponent> hitEntityHealthComponent = g_gameClient->GetComponentManager().GetComponent<HealthComponent>(hitInfo.m_entity);
                    if (!hitEntityHealthComponent.expired()) {
                        // TODO: spawn blood effect
                    }
                }
            }

            clientComponent.m_isLastShootInputPending = false;
        }
    }

    return true;
}

//----------------------------------------------------------------------------------------------------
bool WeaponSystemClient::Render()
{
    OPTICK_EVENT();

    RendererComponent& rendererComponent = g_gameClient->GetRendererComponent();
    LinkingContext& linkingContext = g_gameClient->GetLinkingContext();
    ClientComponent& clientComponent = g_gameClient->GetClientComponent();
    for (const auto& entity : m_entities) {
        NetworkID networkID = linkingContext.GetNetworkID(entity);
        if (networkID >= INVALID_NETWORK_ID) {
            continue;
        }

        std::weak_ptr<WeaponComponent> weaponComponent = g_gameClient->GetComponentManager().GetComponent<WeaponComponent>(entity);
        if (!weaponComponent.lock()->m_hasShot) {
            continue;
        }

        glm::vec4 color = White;
        if (clientComponent.IsLocalEntity(entity)) {
            switch (clientComponent.m_playerID) {
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
        } else {
            switch (clientComponent.m_playerID) {
            case 0: {
                color = Blue;
                break;
            }
            case 1: {
                color = Red;
                break;
            }
            default: {
                break;
            }
            }
        }
        Draw(rendererComponent, weaponComponent, color);
    }

    return true;
}
}
