#include "StatesPanel.h"

#include "FSM.h"
#include "GameClient.h"
#include "GameplayComponent.h"
#include "MainMenuComponent.h"
#include "ScoreboardComponent.h"
#include "State.h"

namespace sand {

//----------------------------------------------------------------------------------------------------
bool StatesPanel::RenderBody() const
{
    ImGui::Text("%s", GetName().c_str());
    ImGui::Separator();
    ImGui::Spacing();

    std::weak_ptr<State> state = g_gameClient->GetFSM().GetCurrentState();
    if (!state.expired()) {
        std::string name = state.lock()->GetName();
        ImGui::Text(name.c_str());

        ImGui::Spacing();
        ImGui::SameLine();
        if (name == "Main Menu") {
            std::weak_ptr<State> mainMenuState = g_gameClient->GetMainMenuComponent().lock()->m_fsm.GetCurrentState();
            if (!mainMenuState.expired()) {
                std::string subName = mainMenuState.lock()->GetName();
                ImGui::Text(subName.c_str());
            } else {
                ImGui::Text("No substate...");
            }
        } else if (name == "Gameplay") {
            std::weak_ptr<State> gameplayState = g_gameClient->GetGameplayComponent().lock()->m_fsm.GetCurrentState();
            if (!gameplayState.expired()) {
                std::string subName = gameplayState.lock()->GetName();
                ImGui::Text(subName.c_str());
            } else {
                ImGui::Text("No substate...");
            }
        } else if (name == "Scoreboard") {
            std::weak_ptr<State> scoreboardState = g_gameClient->GetScoreboardComponent().lock()->m_fsm.GetCurrentState();
            if (!scoreboardState.expired()) {
                std::string subName = scoreboardState.lock()->GetName();
                ImGui::Text(subName.c_str());
            } else {
                ImGui::Text("No substate...");
            }
        } else {
            ImGui::Text("No substate...");
        }
    } else {
        ImGui::Text("No state...");
    }

    return true;
}
}
