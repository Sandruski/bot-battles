#include "SightSystemClient.h"

#include "ComponentManager.h"
#include "GameClient.h"
#include "LinkingContext.h"
#include "RendererComponent.h"
#include "SightComponent.h"
#include "TransformComponent.h"

namespace sand {

//----------------------------------------------------------------------------------------------------
SightSystemClient::SightSystemClient()
{
    m_signature |= 1 << static_cast<U16>(ComponentType::TRANSFORM);
    m_signature |= 1 << static_cast<U16>(ComponentType::SIGHT);
    m_signature |= 1 << static_cast<U16>(ComponentType::LOCAL_PLAYER);
}

//----------------------------------------------------------------------------------------------------
bool SightSystemClient::DebugRender()
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
        std::weak_ptr<SightComponent> sightComponent = g_gameClient->GetComponentManager().GetComponent<SightComponent>(entity);
        if (!transformComponent.lock()->m_isEnabled || !sightComponent.lock()->m_isEnabled) {
            continue;
        }

        DebugDraw(rendererComponent, transformComponent, sightComponent);
    }

    return true;
}
}
