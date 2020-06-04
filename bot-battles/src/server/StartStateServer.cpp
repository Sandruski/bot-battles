#include "StartStateServer.h"

#include "FSM.h"
#include "GameServer.h"
#include "GameplayComponent.h"
#include "ServerComponent.h"
#include "WindowComponent.h"

namespace sand {

//----------------------------------------------------------------------------------------------------
std::string StartStateServer::GetName() const
{
    return "Start";
}

//----------------------------------------------------------------------------------------------------
bool StartStateServer::Enter() const
{
    ILOG("Entering %s...", GetName().c_str());

    return true;
}

//----------------------------------------------------------------------------------------------------
bool StartStateServer::RenderGui() const
{
    ImGuiWindowFlags windowFlags = 0;
    windowFlags |= ImGuiWindowFlags_NoResize;
    windowFlags |= ImGuiWindowFlags_NoMove;
    windowFlags |= ImGuiWindowFlags_NoScrollbar;
    windowFlags |= ImGuiWindowFlags_NoCollapse;
    windowFlags |= ImGuiWindowFlags_NoSavedSettings;

    std::weak_ptr<WindowComponent> windowComponent = g_gameServer->GetWindowComponent();
    ImVec2 position = ImVec2(static_cast<F32>(windowComponent.lock()->m_currentResolution.x) / 2.0f, static_cast<F32>(windowComponent.lock()->m_currentResolution.y) / 2.0f);
    ImGui::SetNextWindowPos(position, ImGuiCond_Always, ImVec2(0.5f, 0.5f));
    ImVec2 size = ImVec2(static_cast<F32>(windowComponent.lock()->m_currentResolution.y) / 2.0f, static_cast<F32>(windowComponent.lock()->m_currentResolution.x) / 2.0f);
    ImGui::SetNextWindowSize(size, ImGuiCond_Always);

    if (ImGui::Begin(GetName().c_str(), nullptr, windowFlags)) {
        std::weak_ptr<ServerComponent> serverComponent = g_gameServer->GetServerComponent();
        const std::unordered_map<PlayerID, std::shared_ptr<ClientProxy>>& playerIDToClientProxy = serverComponent.lock()->GetPlayerIDToClientProxyMap();
        for (const auto& pair : playerIDToClientProxy) {
            PlayerID playerID = pair.first;
            U32 playerNumber = playerID + 1;
            ImGui::Text("Player %u", playerNumber);
        }

        const char* mainMenu = "Main menu";
        ImVec2 mainMenuTextSize = ImGui::CalcTextSize(mainMenu);
        ImVec2 framePadding = ImGui::GetStyle().FramePadding;
        ImVec2 mainMenuButtonSize = ImVec2(mainMenuTextSize.x + framePadding.x * 2.0f, mainMenuTextSize.y + framePadding.y * 2.0f);
        ImVec2 contentRegionMax = ImGui::GetWindowContentRegionMax();
        ImVec2 itemSpacing = ImGui::GetStyle().ItemSpacing;
        ImGui::SetCursorPosX(contentRegionMax.x - mainMenuButtonSize.x);
        ImGui::SetCursorPosY(contentRegionMax.y - mainMenuButtonSize.y);
        // X
        if (ImGui::Button(mainMenu)) {
            Event newEvent;
            newEvent.eventType = EventType::SEND_BYE;
            g_gameServer->GetFSM().NotifyEvent(newEvent);

            ChangeToMainMenu();
        }

        ImGui::End();
    }

    return true;
}

//----------------------------------------------------------------------------------------------------
bool StartStateServer::Exit() const
{
    ILOG("Exiting %s...", GetName().c_str());

    return true;
}

//----------------------------------------------------------------------------------------------------
void StartStateServer::OnNotify(const Event& event)
{
    switch (event.eventType) {

    case EventType::PLAYER_ADDED: {
        OnPlayerAdded();
        break;
    }

    case EventType::HELLO_RECEIVED: {
        Event newEvent;
        newEvent.eventType = EventType::SEND_WELCOME;
        newEvent.networking.playerID = event.networking.playerID;
        g_gameServer->GetFSM().NotifyEvent(newEvent);
        break;
    }

    case EventType::REHELLO_RECEIVED: {
        Event newEvent;
        newEvent.eventType = EventType::SEND_REWELCOME;
        newEvent.networking.playerID = event.networking.playerID;
        g_gameServer->GetFSM().NotifyEvent(newEvent);
        break;
    }

    default: {
        break;
    }
    }
}

//----------------------------------------------------------------------------------------------------
void StartStateServer::OnPlayerAdded() const
{
    std::weak_ptr<ServerComponent> serverComponent = g_gameServer->GetServerComponent();
    U32 entityCount = serverComponent.lock()->GetEntityCount();
    // V
    if (entityCount == MAX_PLAYER_IDS) {
        ChangeToPlay();
    }
}

//----------------------------------------------------------------------------------------------------
void StartStateServer::ChangeToPlay() const
{
    std::weak_ptr<GameplayComponent> gameplayComponent = g_gameServer->GetGameplayComponent();
    gameplayComponent.lock()->m_fsm.ChangeState("Play");
}

//----------------------------------------------------------------------------------------------------
void StartStateServer::ChangeToMainMenu() const
{
    g_gameServer->GetFSM().ChangeState("Main Menu");
}
}
