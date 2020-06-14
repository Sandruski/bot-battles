#include "WeaponSystem.h"

#include "Game.h"
#include "RendererComponent.h"
#include "WeaponComponent.h"
#include "WindowComponent.h"

namespace sand {

//----------------------------------------------------------------------------------------------------
void WeaponSystem::Draw(PlayerID playerID, std::weak_ptr<WeaponComponent> weaponComponent) const
{
    std::weak_ptr<RendererComponent> rendererComponent = g_game->GetRendererComponent();

    glm::vec2 fromPosition = weaponComponent.lock()->m_originLastShot;
    glm::vec2 toPosition = weaponComponent.lock()->m_destinationLastShot;
    glm::vec4 color = White;
    switch (playerID) {
    case 0: {
        color = Red;
        break;
    }
    case 1: {
        color = Blue;
        break;
    }
    default: {
        break;
    }
    }
    color.a = weaponComponent.lock()->m_alpha;

    glLineWidth(1.5f);
    rendererComponent.lock()->DrawLine(fromPosition, toPosition, static_cast<F32>(LayerType::WEAPON), color);
    glLineWidth(1.0f);
}

//----------------------------------------------------------------------------------------------------
void WeaponSystem::DrawGui(PlayerID playerID, std::weak_ptr<WeaponComponent> weaponComponent) const
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

    ImVec2 windowPosition = ImVec2(0.0f, 45.0f);
    ImVec2 windowPivot = ImVec2(0.0f, 0.5f);
    switch (playerID) {
    case 0: {
        F32 positionX = static_cast<F32>(windowComponent.lock()->m_baseResolution.x) / 4.0f;
        F32 offsetX = static_cast<F32>(windowComponent.lock()->m_baseResolution.x) / 3.0f / 2.0f;
        windowPosition.x = positionX - offsetX;
        windowPivot.x = 0.0f;
        break;
    }
    case 1: {
        F32 positionX = static_cast<F32>(windowComponent.lock()->m_baseResolution.x) - static_cast<F32>(windowComponent.lock()->m_baseResolution.x) / 4.0f;
        F32 offsetX = static_cast<F32>(windowComponent.lock()->m_baseResolution.x) / 3.0f / 2.0f;
        windowPosition.x = positionX + offsetX;
        windowPivot.x = 1.0f;
        break;
    }
    default: {
        break;
    }
    }
    windowPosition.x *= proportion.x;
    windowPosition.y *= proportion.y;
    ImGui::SetNextWindowPos(windowPosition, ImGuiCond_Always, windowPivot);

    std::string windowName = "##ammo";
    windowName.append(std::to_string(playerID));
    if (ImGui::Begin(windowName.c_str(), nullptr, windowFlags)) {
        glm::vec4 color = White;
        switch (playerID) {
        case 0: {
            color = Red;
            break;
        }
        case 1: {
            color = Blue;
            break;
        }
        default: {
            break;
        }
        }
        ImVec4 colorText = ImVec4(color.r, color.g, color.b, color.a);

        std::string text = std::to_string(weaponComponent.lock()->m_currentAmmoPrimary);
        text.append("/");
        text.append(std::to_string(weaponComponent.lock()->m_ammoPrimary));

        ImGui::TextColored(colorText, text.c_str());

        ImGui::End();
    }
}
}
