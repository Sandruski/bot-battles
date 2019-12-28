#include "InputSystemServer.h"

#include "ClientProxy.h"
#include "ComponentManager.h"
#include "GameServer.h"
#include "InputComponent.h"
#include "Move.h"
#include "ServerComponent.h"

namespace sand {

//----------------------------------------------------------------------------------------------------
InputSystemServer::InputSystemServer()
{
    m_signature |= 1 << static_cast<U16>(ComponentType::INPUT);
}

//----------------------------------------------------------------------------------------------------
InputSystemServer::~InputSystemServer()
{
}

//----------------------------------------------------------------------------------------------------
bool InputSystemServer::Update()
{
    ServerComponent& serverComponent = g_gameServer->GetServerComponent();

    for (auto& entity : m_entities) {
        std::shared_ptr<ClientProxy> clientProxy = serverComponent.GetClientProxyFromEntity(entity);
        if (clientProxy == nullptr) {
            continue;
        }

        const std::deque<Move>& unprocessedMoves = clientProxy->GetUnprocessedMoves();
        for (const Move& unprocessedMove : unprocessedMoves) {
            std::weak_ptr<InputComponent> inputComponent = g_gameServer->GetComponentManager().GetComponent<InputComponent>(entity);
            inputComponent.lock()->Copy(unprocessedMove.GetInput());
            //F32 dt = unprocessedMove.GetDt();
        }
        clientProxy->ClearUnprocessedMoves();
    }

    return true;
}
}
