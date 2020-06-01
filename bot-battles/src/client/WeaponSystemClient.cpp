#include "WeaponSystemClient.h"

#include "BotComponent.h"
#include "ComponentManager.h"
#include "ComponentMemberTypes.h"
#include "GameClient.h"
#include "LinkingContext.h"
#include "RendererComponent.h"
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

        std::weak_ptr<WeaponComponent> weaponComponent = g_gameClient->GetComponentManager().GetComponent<WeaponComponent>(entity);
        std::weak_ptr<BotComponent> botComponent = g_gameClient->GetComponentManager().GetComponent<BotComponent>(entity);
        std::weak_ptr<SpriteComponent> spriteComponent = g_gameClient->GetComponentManager().GetComponent<SpriteComponent>(entity);

        if (clientComponent.m_isLastWeaponInputPending) {
            clientComponent.m_isLastWeaponInputPending = false;

            if (!botComponent.lock()->m_canPerformAction) {
                continue;
            }

            const Input& input = clientComponent.m_inputBuffer.GetLast();
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
