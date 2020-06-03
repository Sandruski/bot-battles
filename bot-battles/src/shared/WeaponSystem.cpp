#include "WeaponSystem.h"

#include "Game.h"
#include "RendererComponent.h"
#include "WeaponComponent.h"
#include "WindowComponent.h"

namespace sand {

//----------------------------------------------------------------------------------------------------
void WeaponSystem::Draw(RendererComponent& rendererComponent, std::weak_ptr<WeaponComponent> weaponComponent, const glm::vec4& color) const
{
    glm::vec3 fromPosition = glm::vec3(weaponComponent.lock()->m_originLastShot.x, weaponComponent.lock()->m_originLastShot.y, static_cast<F32>(LayerType::WEAPON));
    glm::vec3 toPosition = glm::vec3(weaponComponent.lock()->m_destinationLastShot.x, weaponComponent.lock()->m_destinationLastShot.y, static_cast<F32>(LayerType::WEAPON));
    glLineWidth(1.4f);
    rendererComponent.DrawLine(fromPosition, toPosition, color);
    glLineWidth(1.0f);
}

//----------------------------------------------------------------------------------------------------
void WeaponSystem::DrawGui(PlayerID playerID, std::weak_ptr<WeaponComponent> weaponComponent, const glm::vec4& color) const
{
    WindowComponent& windowComponent = g_game->GetWindowComponent();

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

    ImVec2 position = ImVec2(0.0f, 40.0f);
    switch (playerID) {
    case 0: {
        F32 positionX = static_cast<F32>(windowComponent.m_currentResolution.x) / 4.0f;
        position.x = positionX - static_cast<F32>(windowComponent.m_currentResolution.x) / 3.0f / 2.0f;
        ImGui::SetNextWindowPos(position, ImGuiCond_Always, ImVec2(0.0f, 0.5f));
        break;
    }
    case 1: {
        F32 positionX = static_cast<F32>(windowComponent.m_currentResolution.x) - static_cast<F32>(windowComponent.m_currentResolution.x) / 4.0f;
        position.x = positionX + static_cast<F32>(windowComponent.m_currentResolution.x) / 3.0f / 2.0f;
        ImGui::SetNextWindowPos(position, ImGuiCond_Always, ImVec2(1.0f, 0.5f));
        break;
    }
    default: {
        break;
    }
    }

    std::string name = "##ammo";
    name.append(std::to_string(playerID));
    if (ImGui::Begin(name.c_str(), nullptr, windowFlags)) {
        ImVec4 colorText = ImVec4(color.r, color.g, color.b, color.a);

        std::string text = std::to_string(weaponComponent.lock()->m_currentAmmoPrimary);
        text.append("/");
        text.append(std::to_string(weaponComponent.lock()->m_ammoPrimary));

        ImGui::TextColored(colorText, text.c_str());

        ImGui::End();
    }
}
}
