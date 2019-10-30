#include "GameplayState.h"

#include "Game.h"
#include "ResourceManager.h"

#include "ComponentManager.h"
#include "EntityManager.h"

#include "TransformComponent.h"
#include "SingletonInputComponent.h"

#include "WorkingDirDefs.h"

namespace sand {

//----------------------------------------------------------------------------------------------------
GameplayState::GameplayState()
{
}

//----------------------------------------------------------------------------------------------------
GameplayState::~GameplayState()
{
}

//----------------------------------------------------------------------------------------------------
bool GameplayState::Create()
{

    return true;
}

//----------------------------------------------------------------------------------------------------
bool GameplayState::Destroy()
{
    return true;
}

//----------------------------------------------------------------------------------------------------
bool GameplayState::Enter()
{
    return true;
}

//----------------------------------------------------------------------------------------------------
bool GameplayState::PreUpdate(F32 /*dt*/)
{
	return true;
}

//----------------------------------------------------------------------------------------------------
bool GameplayState::Update(F32 /*dt*/)
{
    return true;
}

//----------------------------------------------------------------------------------------------------
bool GameplayState::PostUpdate(F32 /*dt*/)
{
    return true;
}

//----------------------------------------------------------------------------------------------------
bool GameplayState::Render()
{
    return true;
}

//----------------------------------------------------------------------------------------------------
bool GameplayState::Exit()
{
    return true;
}
}
