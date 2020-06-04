#include "WeaponSystemClient.h"

#include "BotComponent.h"
#include "ClientComponent.h"
#include "ComponentManager.h"
#include "ComponentMemberTypes.h"
#include "GameClient.h"
#include "GameplayComponent.h"
#include "LinkingContext.h"
#include "SpriteComponent.h"
#include "WeaponComponent.h"

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

    std::weak_ptr<GameplayComponent> gameplayComponent = g_gameClient->GetGameplayComponent();
    std::weak_ptr<State> currentState = gameplayComponent.lock()->m_fsm.GetCurrentState();
    if (currentState.expired()) {
        return true;
    }

    std::weak_ptr<ClientComponent> clientComponent = g_gameClient->GetClientComponent();
    if (!clientComponent.lock()->m_isClientPrediction) {
        return true;
    }

    for (auto& entity : m_entities) {
        if (g_gameClient->GetLinkingContext().GetNetworkID(entity) >= INVALID_NETWORK_ID) {
            continue;
        }

        if (!clientComponent.lock()->IsLocalEntity(entity)) {
            continue;
        }

        std::weak_ptr<WeaponComponent> weaponComponent = g_gameClient->GetComponentManager().GetComponent<WeaponComponent>(entity);
        std::weak_ptr<BotComponent> botComponent = g_gameClient->GetComponentManager().GetComponent<BotComponent>(entity);
        std::weak_ptr<SpriteComponent> spriteComponent = g_gameClient->GetComponentManager().GetComponent<SpriteComponent>(entity);

        if (clientComponent.lock()->m_isLastWeaponInputPending) {
            clientComponent.lock()->m_isLastWeaponInputPending = false;

            if (botComponent.lock()->m_actionType != BotComponent::ActionType::NONE) {
                continue;
            }

            const Input& input = clientComponent.lock()->m_inputBuffer.GetLast();
            U64 dirtyState = input.GetDirtyState();

            // Reload
            const bool hasReload = dirtyState & static_cast<U64>(InputComponentMemberType::INPUT_RELOAD);
            if (hasReload) {
                const bool result = weaponComponent.lock()->CanReload();
                if (!result) {
                    continue;
                }

                spriteComponent.lock()->m_spriteName = "reload";
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

                    spriteComponent.lock()->m_spriteName = "shootPrimary";
                } else {
                    const bool result = weaponComponent.lock()->CanShootSecondary();
                    if (!result) {
                        continue;
                    }

                    spriteComponent.lock()->m_spriteName = "shootSecondary";
                }
            }
        }
    }

    return true;
}

//----------------------------------------------------------------------------------------------------
bool WeaponSystemClient::Render()
{
    OPTICK_EVENT();

    std::weak_ptr<ClientComponent> clientComponent = g_gameClient->GetClientComponent();
    LinkingContext& linkingContext = g_gameClient->GetLinkingContext();
    for (const auto& entity : m_entities) {
        NetworkID networkID = linkingContext.GetNetworkID(entity);
        if (networkID >= INVALID_NETWORK_ID) {
            continue;
        }

        std::weak_ptr<BotComponent> botComponent = g_gameClient->GetComponentManager().GetComponent<BotComponent>(entity);
        if (botComponent.lock()->m_actionType != BotComponent::ActionType::SHOOT) {
            continue;
        }

        PlayerID playerID = INVALID_PLAYER_ID;
        const bool isLocalEntity = clientComponent.lock()->IsLocalEntity(entity);
        if (isLocalEntity) {
            playerID = clientComponent.lock()->m_playerID;
        } else {
            switch (clientComponent.lock()->m_playerID) {
            case 0: {
                playerID = 1;
                break;
            }
            case 1: {
                playerID = 0;
                break;
            }
            default: {
                break;
            }
            }
        }
        std::weak_ptr<WeaponComponent> weaponComponent = g_gameClient->GetComponentManager().GetComponent<WeaponComponent>(entity);
        Draw(playerID, weaponComponent);
    }

    return true;
}

//----------------------------------------------------------------------------------------------------
bool WeaponSystemClient::RenderGui()
{
    OPTICK_EVENT();

    std::weak_ptr<ClientComponent> clientComponent = g_gameClient->GetClientComponent();
    LinkingContext& linkingContext = g_gameClient->GetLinkingContext();
    for (const auto& entity : m_entities) {
        NetworkID networkID = linkingContext.GetNetworkID(entity);
        if (networkID >= INVALID_NETWORK_ID) {
            continue;
        }

        PlayerID playerID = INVALID_PLAYER_ID;
        const bool isLocalEntity = clientComponent.lock()->IsLocalEntity(entity);
        if (isLocalEntity) {
            playerID = clientComponent.lock()->m_playerID;
        } else {
            switch (clientComponent.lock()->m_playerID) {
            case 0: {
                playerID = 1;
                break;
            }
            case 1: {
                playerID = 0;
                break;
            }
            default: {
                break;
            }
            }
        }
        std::weak_ptr<WeaponComponent> weaponComponent = g_gameClient->GetComponentManager().GetComponent<WeaponComponent>(entity);
        DrawGui(playerID, weaponComponent);
    }

    return true;
}
}
