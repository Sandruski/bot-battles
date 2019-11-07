#include "NavigationSystem.h"

#include "ClientProxy.h"
#include "Game.h"
#include "Move.h"
#include "MoveList.h"
#include "SingletonServerComponent.h"

namespace sand {

//----------------------------------------------------------------------------------------------------
NavigationSystem::NavigationSystem()
{
}

//----------------------------------------------------------------------------------------------------
NavigationSystem::~NavigationSystem()
{
}

//----------------------------------------------------------------------------------------------------
bool NavigationSystem::StartUp()
{
    return false;
}

//----------------------------------------------------------------------------------------------------
bool NavigationSystem::Update()
{ /*
    std::shared_ptr<SingletonServerComponent> server = g_game->GetSingletonServerComponent();

    // TODO: loop all clients? = All entities with the component clientblablabla
    U32 playerID = 0; // TODO: where does this come from?

    std::shared_ptr<ClientProxy> clientProxy = server->m_playerIDToClientProxy.at(playerID);
    MoveList& unprocessedMoves = clientProxy->GetUnprocessedMoveList();
    std::deque<Move> moves = unprocessedMoves.GetMoves();
    for (const Move& unprocessedMove : moves) {
        const InputState& inputState = unprocessedMove.GetInputState();
        float dt = unprocessedMove.GetDt();
        ProcessInput(dt, inputState);
        UpdateMovement(dt);
    }

    unprocessedMoves.Clear();*/

    return true;
}

//----------------------------------------------------------------------------------------------------
bool NavigationSystem::ShutDown()
{
    return false;
}

//----------------------------------------------------------------------------------------------------
void NavigationSystem::ProcessInput(F32 /*dt*/, const InputState& /*inputState*/) const
{
}

//----------------------------------------------------------------------------------------------------
void NavigationSystem::UpdateMovement(F32 /*dt*/) const
{
}
}
