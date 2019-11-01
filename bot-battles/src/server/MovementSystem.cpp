#include "MovementSystem.h"

#include "ClientProxy.h"
#include "Game.h"
#include "Move.h"
#include "MoveList.h"
#include "SingletonServerComponent.h"

namespace sand {

//----------------------------------------------------------------------------------------------------
MovementSystem::MovementSystem()
{
}

//----------------------------------------------------------------------------------------------------
MovementSystem::~MovementSystem()
{
}

//----------------------------------------------------------------------------------------------------
bool MovementSystem::StartUp()
{
    return false;
}

//----------------------------------------------------------------------------------------------------
bool MovementSystem::Update()
{
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

    unprocessedMoves.Clear();
}

//----------------------------------------------------------------------------------------------------
bool MovementSystem::ShutDown()
{
    return false;
}

//----------------------------------------------------------------------------------------------------
void MovementSystem::ProcessInput(F32 /*dt*/, const InputState& /*inputState*/) const
{
}

//----------------------------------------------------------------------------------------------------
void MovementSystem::UpdateMovement(F32 /*dt*/) const
{
}
}
