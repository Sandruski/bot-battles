#include "ConnectStateServer.h"

#include "ConfigServer.h"
#include "FSM.h"
#include "GameServer.h"
#include "ServerComponent.h"

namespace sand {

//----------------------------------------------------------------------------------------------------
std::string ConnectStateServer::GetName() const
{
    return "Connect";
}

//----------------------------------------------------------------------------------------------------
bool ConnectStateServer::Enter() const
{
    ILOG("Entering %s...", GetName().c_str());

    Event newEvent;
    newEvent.eventType = EventType::CONNECT_SOCKETS;
    g_gameServer->GetFSM().NotifyEvent(newEvent);

    return true;
}

//----------------------------------------------------------------------------------------------------
bool ConnectStateServer::Update() const
{
    return true;
}

//----------------------------------------------------------------------------------------------------
bool ConnectStateServer::Exit() const
{
    ILOG("Exiting %s...", GetName().c_str());

    return true;
}

//----------------------------------------------------------------------------------------------------
void ConnectStateServer::OnNotify(const Event& event)
{
    switch (event.eventType) {

        // V
    case EventType::SOCKETS_CONNECTED: {
        ChangeToGameplay();
        break;
    }

    default: {
        break;
    }
    }
}

//----------------------------------------------------------------------------------------------------
void ConnectStateServer::ChangeToGameplay() const
{
    g_gameServer->GetFSM().ChangeState("Gameplay");
}
}
