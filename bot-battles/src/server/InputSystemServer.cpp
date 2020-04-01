#include "InputSystemServer.h"

#include "ClientProxy.h"
#include "GameServer.h"
#include "ServerComponent.h"
#include "State.h"

namespace sand {

//----------------------------------------------------------------------------------------------------
bool InputSystemServer::Update()
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
        clientProxy.lock()->m_inputBuffer.Clear();
    }

    return true;
}
}
