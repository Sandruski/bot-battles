#include "PlayStateClient.h"

#include "ConfigClient.h"
#include "EntityManager.h"
#include "EventComponent.h"
#include "FSM.h"
#include "GameClient.h"
#include "GameplayComponent.h"
#include "LinkingContext.h"
#include "WindowComponent.h"

namespace sand {

//----------------------------------------------------------------------------------------------------
std::string PlayStateClient::GetName() const
{
    return "Play";
}

//----------------------------------------------------------------------------------------------------
bool PlayStateClient::Enter() const
{
    ILOG("Entering %s...", GetName().c_str());

    return true;
}

//----------------------------------------------------------------------------------------------------
bool PlayStateClient::Update() const
{
    std::weak_ptr<EventComponent> eventComponent = g_game->GetEventComponent();
    if (eventComponent.lock()->m_keyboard.at(SDL_SCANCODE_O) == EventComponent::KeyState::DOWN) {
        std::weak_ptr<GameplayComponent> gameplayComponent = g_gameClient->GetGameplayComponent();
        gameplayComponent.lock()->m_isLog = !gameplayComponent.lock()->m_isLog;
    }

    return true;
}

//----------------------------------------------------------------------------------------------------
bool PlayStateClient::RenderGui() const
{
    std::weak_ptr<GameplayComponent> gameplayComponent = g_gameClient->GetGameplayComponent();
    if (!gameplayComponent.lock()->m_isLog) {
        return true;
    }

    ImGuiWindowFlags windowFlags = 0;
    windowFlags |= ImGuiWindowFlags_NoResize;
    windowFlags |= ImGuiWindowFlags_NoMove;
    windowFlags |= ImGuiWindowFlags_NoCollapse;
    windowFlags |= ImGuiWindowFlags_NoSavedSettings;

    std::weak_ptr<WindowComponent> windowComponent = g_gameClient->GetWindowComponent();
    ImVec2 position = ImVec2(static_cast<F32>(windowComponent.lock()->m_currentResolution.x) / 2.0f, static_cast<F32>(windowComponent.lock()->m_currentResolution.y) / 1.15f);
    ImGui::SetNextWindowPos(position, ImGuiCond_Always, ImVec2(0.5f, 0.5f));
    ImVec2 size = ImVec2(static_cast<F32>(windowComponent.lock()->m_currentResolution.y), static_cast<F32>(windowComponent.lock()->m_currentResolution.x) / 8.0f);
    ImGui::SetNextWindowSize(size, ImGuiCond_Always);

    if (ImGui::Begin("Output", nullptr, windowFlags)) {
        if (ImGui::BeginChild("##console", ImVec2(0, 0), false, windowFlags)) {
            ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0, 0));

            const char* bufBegin = gameplayComponent.lock()->m_buf.begin();
            const char* bufEnd = gameplayComponent.lock()->m_buf.end();
            ImGuiListClipper clipper;
            clipper.Begin(gameplayComponent.lock()->m_lineOffsets.Size);
            while (clipper.Step()) {
                for (I32 lineNumber = clipper.DisplayStart; lineNumber < clipper.DisplayEnd; ++lineNumber) {
                    const char* lineStart = bufBegin + gameplayComponent.lock()->m_lineOffsets[lineNumber];
                    const char* lineEnd = (lineNumber + 1 < gameplayComponent.lock()->m_lineOffsets.Size) ? (bufBegin + gameplayComponent.lock()->m_lineOffsets[lineNumber + 1] - 1) : bufEnd;
                    ImGui::TextUnformatted(lineStart, lineEnd);
                }
            }
            clipper.End();

            ImGui::PopStyleVar();

            if (ImGui::GetScrollY() >= ImGui::GetScrollMaxY()) {
                ImGui::SetScrollHereY(1.0f);
            }

            ImGui::EndChild();
        }

        ImGui::End();
    }

    return true;
}

//----------------------------------------------------------------------------------------------------
bool PlayStateClient::Exit() const
{
    ILOG("Exiting %s...", GetName().c_str());

    std::weak_ptr<GameplayComponent> gameplayComponent = g_gameClient->GetGameplayComponent();
    gameplayComponent.lock()->ClearLogs();

    return true;
}

//----------------------------------------------------------------------------------------------------
void PlayStateClient::OnNotify(const Event& event)
{
    switch (event.eventType) {

        // V
    case EventType::RESULTS_RECEIVED: {
        ChangeToScoreboard();
        break;
    }

        // X
    case EventType::PLAYER_REMOVED: {
        ChangeToMainMenu();
        break;
    }

    default: {
        break;
    }
    }
}

//----------------------------------------------------------------------------------------------------
void PlayStateClient::ChangeToScoreboard() const
{
    g_gameClient->GetFSM().ChangeState("Scoreboard");
}

//----------------------------------------------------------------------------------------------------
void PlayStateClient::ChangeToMainMenu() const
{
    g_gameClient->GetFSM().ChangeState("Main Menu");
}
}
