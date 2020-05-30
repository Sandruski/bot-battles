#include "HUDSystem.h"

#include "ComponentManager.h"
#include "Game.h"
#include "LabelComponent.h"
#include "TransformComponent.h"
#include "WindowComponent.h"

namespace sand {

//----------------------------------------------------------------------------------------------------
HUDSystem::HUDSystem()
{
    m_signature |= 1 << static_cast<U16>(ComponentType::LABEL);
}

//----------------------------------------------------------------------------------------------------
bool HUDSystem::RenderGui()
{
    OPTICK_EVENT();

    WindowComponent& windowComponent = g_game->GetWindowComponent();
    glm::vec2 proportion = windowComponent.GetProportion();

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

    for (auto& entity : m_entities) {
        std::weak_ptr<TransformComponent> transformComponent = g_game->GetComponentManager().GetComponent<TransformComponent>(entity);
        std::weak_ptr<LabelComponent> labelComponent = g_game->GetComponentManager().GetComponent<LabelComponent>(entity);

        glm::vec2 finalPosition = transformComponent.lock()->m_position + labelComponent.lock()->m_offset;
        finalPosition *= proportion;
        ImVec2 position = ImVec2(finalPosition.x, finalPosition.y);
        ImGui::SetNextWindowPos(position, ImGuiCond_Always, ImVec2(0.5f, 0.5f));

        std::string name = "##";
        name.append(labelComponent.lock()->m_text);
        if (ImGui::Begin(name.c_str(), nullptr, windowFlags)) {
            ImVec4 color = ImVec4(labelComponent.lock()->m_color.r, labelComponent.lock()->m_color.g, labelComponent.lock()->m_color.b, labelComponent.lock()->m_color.a);
            ImGui::TextColored(color, labelComponent.lock()->m_text.c_str());

            ImGui::End();
        }
    }

    return true;
}
}
