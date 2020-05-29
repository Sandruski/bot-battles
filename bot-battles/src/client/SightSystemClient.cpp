#include "SightSystemClient.h"

#include "ComponentManager.h"
#include "ComponentMemberTypes.h"
#include "GameClient.h"
#include "LinkingContext.h"
#include "RendererComponent.h"
#include "SightComponent.h"
#include "SpriteComponent.h"
#include "TransformComponent.h"

namespace sand {

//----------------------------------------------------------------------------------------------------
SightSystemClient::SightSystemClient()
{
    m_signature |= 1 << static_cast<U16>(ComponentType::TRANSFORM);
    m_signature |= 1 << static_cast<U16>(ComponentType::SIGHT);
    m_signature |= 1 << static_cast<U16>(ComponentType::BOT);
    m_signature |= 1 << static_cast<U16>(ComponentType::LOCAL_PLAYER);
}

//----------------------------------------------------------------------------------------------------
bool SightSystemClient::Render()
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

        glm::vec4 color = Red;
        color.a = 0.5f;
        Draw(rendererComponent, transformComponent, sightComponent, color);
    }

    return true;
}

//----------------------------------------------------------------------------------------------------
void SightSystemClient::OnNotify(const Event& event)
{
    switch (event.eventType) {

    case EventType::SEEN_NEW_ENTITY: {
        OnSeenNewEntity(event.sight.entity);
        break;
    }

    case EventType::SEEN_LOST_ENTITY: {
        OnSeenLostEntity(event.sight.entity);
        break;
    }

    default: {
        break;
    }
    }
}

//----------------------------------------------------------------------------------------------------
void SightSystemClient::OnSeenNewEntity(Entity seenEntity) const
{
    assert(seenEntity < INVALID_ENTITY);

    LinkingContext& linkingContext = g_gameClient->GetLinkingContext();
    for (const auto& entity : m_entities) {
        NetworkID networkID = linkingContext.GetNetworkID(entity);
        if (networkID >= INVALID_NETWORK_ID) {
            continue;
        }

        std::weak_ptr<SightComponent> sightComponent = g_gameClient->GetComponentManager().GetComponent<SightComponent>(entity);
        sightComponent.lock()->m_seenEntities.emplace_back(seenEntity);

        std::weak_ptr<SpriteComponent> seenSpriteComponent = g_gameClient->GetComponentManager().GetComponent<SpriteComponent>(seenEntity);
        if (!seenSpriteComponent.expired()) {
            seenSpriteComponent.lock()->m_pct = 0.0f;

            Event newComponentEvent;
            newComponentEvent.eventType = EventType::COMPONENT_MEMBER_CHANGED;
            newComponentEvent.component.dirtyState = static_cast<U64>(ComponentMemberType::SPRITE_PCT);
            newComponentEvent.component.entity = seenEntity;
            NotifyEvent(newComponentEvent);
        }
    }
}

//----------------------------------------------------------------------------------------------------
void SightSystemClient::OnSeenLostEntity(Entity seenEntity) const
{
    assert(seenEntity < INVALID_ENTITY);

    LinkingContext& linkingContext = g_gameClient->GetLinkingContext();
    for (const auto& entity : m_entities) {
        NetworkID networkID = linkingContext.GetNetworkID(entity);
        if (networkID >= INVALID_NETWORK_ID) {
            continue;
        }

        std::weak_ptr<SightComponent> sightComponent = g_gameClient->GetComponentManager().GetComponent<SightComponent>(entity);
        std::vector<Entity>::const_iterator it = std::find(sightComponent.lock()->m_seenEntities.begin(), sightComponent.lock()->m_seenEntities.end(), seenEntity);
        sightComponent.lock()->m_seenEntities.erase(it);

        std::weak_ptr<SpriteComponent> seenSpriteComponent = g_gameClient->GetComponentManager().GetComponent<SpriteComponent>(seenEntity);
        if (!seenSpriteComponent.expired()) {
            seenSpriteComponent.lock()->m_color = Black;
            seenSpriteComponent.lock()->m_pct = 0.5f;

            Event newComponentEvent;
            newComponentEvent.eventType = EventType::COMPONENT_MEMBER_CHANGED;
            newComponentEvent.component.dirtyState = static_cast<U64>(ComponentMemberType::SPRITE_COLOR) | static_cast<U64>(ComponentMemberType::SPRITE_PCT);
            newComponentEvent.component.entity = seenEntity;
            NotifyEvent(newComponentEvent);
        }
    }
}
}
