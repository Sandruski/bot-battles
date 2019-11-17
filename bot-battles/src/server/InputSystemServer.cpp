#include "InputSystemServer.h"

#include "ClientProxy.h"
#include "ComponentManager.h"
#include "Game.h"
#include "InputComponent.h"
#include "Move.h"
#include "SingletonServerComponent.h"

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
    std::shared_ptr<SingletonServerComponent> singletonServer = g_game->GetSingletonServerComponent();

    for (auto& entity : m_entities) {
        std::shared_ptr<ClientProxy> clientProxy = singletonServer->GetClientProxyFromEntity(entity);
        const std::deque<Move>& unprocessedMoves = clientProxy->GetUnprocessedMoves();
        for (const Move& unprocessedMove : unprocessedMoves) {
            std::shared_ptr<InputComponent> input = g_game->GetComponentManager().GetComponent<InputComponent>(entity);
            input->Copy(unprocessedMove.GetInput());
            //F32 dt = unprocessedMove.GetDt();
        }
        clientProxy->ClearUnprocessedMoves();
    }

    return true;
}
}
