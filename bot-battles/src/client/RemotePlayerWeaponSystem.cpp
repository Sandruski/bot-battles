#include "RemotePlayerWeaponSystem.h"

#include "BotComponent.h"
#include "ClientComponent.h"
#include "ComponentManager.h"
#include "GameClient.h"
#include "LinkingContext.h"
#include "WeaponComponent.h"

namespace sand {

//----------------------------------------------------------------------------------------------------
RemotePlayerWeaponSystem::RemotePlayerWeaponSystem()
{
    m_signature |= 1 << static_cast<U16>(ComponentType::WEAPON);
    m_signature |= 1 << static_cast<U16>(ComponentType::PLAYER);
    m_signature |= 1 << static_cast<U16>(ComponentType::REMOTE_PLAYER);
}

//----------------------------------------------------------------------------------------------------
bool RemotePlayerWeaponSystem::Render()
{
    OPTICK_EVENT();

    F32 dt = MyTime::GetInstance().GetDt();

    std::weak_ptr<ClientComponent> clientComponent = g_gameClient->GetClientComponent();
    LinkingContext& linkingContext = g_gameClient->GetLinkingContext();
    for (const auto& entity : m_entities) {
        NetworkID networkID = linkingContext.GetNetworkID(entity);
        if (networkID >= INVALID_NETWORK_ID) {
            continue;
        }

        std::weak_ptr<BotComponent> botComponent = g_gameClient->GetComponentManager().GetComponent<BotComponent>(entity);
        if (botComponent.lock()->m_actionType != BotComponent::ActionType::SHOOT_PRIMARY
            && botComponent.lock()->m_actionType != BotComponent::ActionType::SHOOT_SECONDARY) {
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
        weaponComponent.lock()->m_alpha -= dt / botComponent.lock()->m_timeAction;
        if (weaponComponent.lock()->m_alpha <= 0.0f) {
            weaponComponent.lock()->m_alpha = 0.0f;
        }
        Draw(playerID, weaponComponent);
    }

    return true;
}

//----------------------------------------------------------------------------------------------------
bool RemotePlayerWeaponSystem::RenderGui()
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
