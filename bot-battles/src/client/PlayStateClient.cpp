#include "PlayStateClient.h"

#include "ClientComponent.h"
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
    ImGuiWindowFlags windowFlags = 0;
    windowFlags |= ImGuiWindowFlags_NoMove;
    windowFlags |= ImGuiWindowFlags_NoScrollWithMouse;
    windowFlags |= ImGuiWindowFlags_AlwaysAutoResize;
    windowFlags |= ImGuiWindowFlags_NoBackground;
    windowFlags |= ImGuiWindowFlags_NoSavedSettings;
    windowFlags |= ImGuiWindowFlags_NoFocusOnAppearing;
    windowFlags |= ImGuiWindowFlags_NoBringToFrontOnFocus;

    windowFlags |= ImGuiWindowFlags_NoDecoration;
    windowFlags |= ImGuiWindowFlags_NoInputs;

    std::weak_ptr<WindowComponent> windowComponent = g_game->GetWindowComponent();
    glm::vec2 proportion = windowComponent.lock()->GetProportion();

    ImVec2 windowPosition = ImVec2(windowComponent.lock()->m_baseResolution.x / 2.0f, 20.0f);
    windowPosition.x *= proportion.x;
    windowPosition.y *= proportion.y;
    ImGui::SetNextWindowPos(windowPosition, ImGuiCond_Always, ImVec2(0.5f, 0.5f));

    if (ImGui::Begin("Time", nullptr, windowFlags)) {
        glm::vec4 color = White;
        ImVec4 colorText = ImVec4(color.r, color.g, color.b, color.a);

        std::weak_ptr<ClientComponent> clientComponent = g_gameClient->GetClientComponent();
        F32 timeDiff = static_cast<F32>(clientComponent.lock()->m_duration) - clientComponent.lock()->m_durationTime;
        std::string text = std::to_string(static_cast<I32>(std::ceil(timeDiff)));

        ImGui::TextColored(colorText, text.c_str());

        ImGui::End();
    }

    std::weak_ptr<GameplayComponent> gameplayComponent = g_gameClient->GetGameplayComponent();
    if (!gameplayComponent.lock()->m_isLog) {
        return true;
    }

    windowFlags = 0;
    windowFlags |= ImGuiWindowFlags_NoResize;
    windowFlags |= ImGuiWindowFlags_NoMove;
    windowFlags |= ImGuiWindowFlags_NoCollapse;
    windowFlags |= ImGuiWindowFlags_NoSavedSettings;

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
