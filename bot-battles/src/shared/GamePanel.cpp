#include "GamePanel.h"

#include "Game.h"
#include "WindowComponent.h"

namespace sand {

//----------------------------------------------------------------------------------------------------
std::string GamePanel::GetName() const
{
    return "Game";
}

//----------------------------------------------------------------------------------------------------
bool GamePanel::RenderHeader() const
{
    return ImGui::Button(GetName().c_str());
}

//----------------------------------------------------------------------------------------------------
bool GamePanel::RenderBody() const
{
    ImGui::Text("%s", GetName().c_str());
    ImGui::Separator();
    ImGui::Spacing();

    F32 fps = MyTime::GetInstance().GetFps();
    const std::vector<F32> fpsTrack = MyTime::GetInstance().GetFpsTrack();
    ImGui::PlotHistogram("FPS", &fpsTrack.front(), static_cast<U32>(fpsTrack.size()), 0, std::to_string(static_cast<U32>(fps)).c_str(), 0.0f, 100.0f, ImVec2(250.0f, 80.0f));

    ImGui::Text("Dt:");
    ImGui::SameLine();
    F32 dt = MyTime::GetInstance().GetDt();
    ImGui::Text("%.2f", dt);

    ImGui::Text("Frame:");
    ImGui::SameLine();
    U32 frame = MyTime::GetInstance().GetFrame();
    ImGui::Text("%u", frame);

    ImGui::Text("Time:");
    ImGui::SameLine();
    F32 time = MyTime::GetInstance().GetTime();
    ImGui::Text("%.f", time);

    return true;
}
}
