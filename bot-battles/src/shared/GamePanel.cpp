#include "GamePanel.h"

namespace sand {

//----------------------------------------------------------------------------------------------------
bool GamePanel::RenderHeader() const
{
    return ImGui::Button("Game");
}

//----------------------------------------------------------------------------------------------------
bool GamePanel::RenderBody() const
{
    F32 fps = MyTime::GetInstance().GetFps();
    const std::vector<F32> fpsTrack = MyTime::GetInstance().GetFpsTrack();
    ImGui::PlotHistogram("FPS", &fpsTrack.front(), fpsTrack.size(), 0, std::to_string(static_cast<U32>(fps)).c_str(), 0.0f, 100.0f, ImVec2(250, 80));

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