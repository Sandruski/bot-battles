#include "HealthSystemClient.h"

#include "BotComponent.h"
#include "ComponentManager.h"
#include "ComponentMemberTypes.h"
#include "GameClient.h"
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

        std::weak_ptr<HealthComponent> healthComponent = g_gameClient->GetComponentManager().GetComponent<HealthComponent>(entity);
        std::weak_ptr<BotComponent> botComponent = g_gameClient->GetComponentManager().GetComponent<BotComponent>(entity);
        std::weak_ptr<SpriteComponent> spriteComponent = g_gameClient->GetComponentManager().GetComponent<SpriteComponent>(entity);

        if (clientComponent.m_isLastHealthInputPending) {
            clientComponent.m_isLastHealthInputPending = false;

            if (botComponent.lock()->m_actionType != BotComponent::ActionType::NONE) {
                continue;
            }

            const Input& input = clientComponent.m_inputBuffer.GetLast();
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

    ClientComponent& clientComponent = g_gameClient->GetClientComponent();
    LinkingContext& linkingContext = g_gameClient->GetLinkingContext();
    for (const auto& entity : m_entities) {
        NetworkID networkID = linkingContext.GetNetworkID(entity);
        if (networkID >= INVALID_NETWORK_ID) {
            continue;
        }

        PlayerID playerID = INVALID_PLAYER_ID;
        const bool isLocalEntity = clientComponent.IsLocalEntity(entity);
        if (isLocalEntity) {
            playerID = clientComponent.m_playerID;
        } else {
            switch (clientComponent.m_playerID) {
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
