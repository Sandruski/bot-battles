#include "HealthSystemClient.h"

#include "BotComponent.h"
#include "ClientComponent.h"
#include "ComponentManager.h"
#include "ComponentMemberTypes.h"
#include "FSM.h"
#include "GameClient.h"
#include "GameplayComponent.h"
#include "HealthComponent.h"
#include "LinkingContext.h"
#include "SpriteComponent.h"

namespace sand {

//----------------------------------------------------------------------------------------------------
HealthSystemClient::HealthSystemClient()
{
    m_signature |= 1 << static_cast<U16>(ComponentType::HEALTH);
    m_signature |= 1 << static_cast<U16>(ComponentType::PLAYER);
}

//----------------------------------------------------------------------------------------------------
bool HealthSystemClient::Update()
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

        std::weak_ptr<HealthComponent> healthComponent = g_gameClient->GetComponentManager().GetComponent<HealthComponent>(entity);
        std::weak_ptr<BotComponent> botComponent = g_gameClient->GetComponentManager().GetComponent<BotComponent>(entity);
        std::weak_ptr<SpriteComponent> spriteComponent = g_gameClient->GetComponentManager().GetComponent<SpriteComponent>(entity);

        if (clientComponent.lock()->m_isLastHealthInputPending) {
            clientComponent.lock()->m_isLastHealthInputPending = false;

            const bool canPerformAction = botComponent.lock()->CanPerformAction();
            if (!canPerformAction) {
                continue;
            }

            const Input& input = clientComponent.lock()->m_inputBuffer.GetLast();
            U64 dirtyState = input.GetDirtyState();

            // Heal
            const bool hasHeal = dirtyState & static_cast<U64>(InputComponentMemberType::INPUT_HEAL);
            if (hasHeal) {
                const bool result = healthComponent.lock()->CanHeal();
                if (!result) {
                    continue;
                }

                spriteComponent.lock()->m_spriteName = "heal";
            }
        }
    }

    return true;
}

//----------------------------------------------------------------------------------------------------
bool HealthSystemClient::Render()
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
        std::weak_ptr<HealthComponent> healthComponent = g_gameClient->GetComponentManager().GetComponent<HealthComponent>(entity);
        Draw(playerID, healthComponent);
    }

    return true;
}
}
