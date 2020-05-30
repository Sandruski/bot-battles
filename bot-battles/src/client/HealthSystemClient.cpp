#include "HealthSystemClient.h"

#include "ComponentManager.h"
#include "GameClient.h"
#include "HealthComponent.h"
#include "LinkingContext.h"
#include "RendererComponent.h"
#include "TransformComponent.h"

namespace sand {

//----------------------------------------------------------------------------------------------------
HealthSystemClient::HealthSystemClient()
{
    m_signature |= 1 << static_cast<U16>(ComponentType::HEALTH);
    m_signature |= 1 << static_cast<U16>(ComponentType::PLAYER);
}

//----------------------------------------------------------------------------------------------------
bool HealthSystemClient::Render()
{
    OPTICK_EVENT();

    RendererComponent& rendererComponent = g_gameClient->GetRendererComponent();
    LinkingContext& linkingContext = g_gameClient->GetLinkingContext();
    for (const auto& entity : m_entities) {
        NetworkID networkID = linkingContext.GetNetworkID(entity);
        if (networkID >= INVALID_NETWORK_ID) {
            continue;
        }

        std::weak_ptr<TransformComponent> transformComponent = g_gameClient->GetComponentManager().GetComponent<TransformComponent>(entity);
        std::weak_ptr<HealthComponent> healthComponent = g_gameClient->GetComponentManager().GetComponent<HealthComponent>(entity);

        glm::vec4 color = White;
        glm::vec4 backgroundColor = Black;
        backgroundColor.a = 0.5f;
        Draw(rendererComponent, transformComponent, healthComponent, color, backgroundColor);
    }

    return true;
}
}
