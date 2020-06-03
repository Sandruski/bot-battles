#include "LabelSystem.h"

#include "ComponentManager.h"
#include "Game.h"
#include "LabelComponent.h"
#include "TransformComponent.h"
#include "WindowComponent.h"

namespace sand {

//----------------------------------------------------------------------------------------------------
LabelSystem::LabelSystem()
{
    m_signature |= 1 << static_cast<U16>(ComponentType::LABEL);
}

//----------------------------------------------------------------------------------------------------
bool LabelSystem::RenderGui()
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

    for (U32 i = 0; i < m_entities.size(); ++i) {
        Entity entity = m_entities.at(i);
        std::weak_ptr<TransformComponent> transformComponent = g_game->GetComponentManager().GetComponent<TransformComponent>(entity);
        std::weak_ptr<LabelComponent> labelComponent = g_game->GetComponentManager().GetComponent<LabelComponent>(entity);

        glm::vec2 finalPosition = transformComponent.lock()->m_position + labelComponent.lock()->m_offset;
        ImVec2 windowPosition = ImVec2(finalPosition.x, finalPosition.y);
        windowPosition.x *= proportion.x;
        windowPosition.y *= proportion.y;
        ImGui::SetNextWindowPos(windowPosition, ImGuiCond_Always, ImVec2(0.5f, 0.5f));

        std::string windowName = "#label";
        windowName.append(std::to_string(i));
        windowName.append(labelComponent.lock()->m_text);
        if (ImGui::Begin(windowName.c_str(), nullptr, windowFlags)) {
            ImVec4 colorText = ImVec4(labelComponent.lock()->m_color.r, labelComponent.lock()->m_color.g, labelComponent.lock()->m_color.b, labelComponent.lock()->m_color.a);

            ImGui::TextColored(colorText, labelComponent.lock()->m_text.c_str());

            ImGui::End();
        }
    }

    return true;
}
}
