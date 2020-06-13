#include "SightSystemClient.h"

#include "ComponentManager.h"
#include "ComponentMemberTypes.h"
#include "GameClient.h"
#include "LinkingContext.h"
#include "SightComponent.h"
#include "SpriteComponent.h"
#include "TransformComponent.h"

namespace sand {

//----------------------------------------------------------------------------------------------------
SightSystemClient::SightSystemClient()
{
    m_signature |= 1 << static_cast<U16>(ComponentType::SIGHT);
    m_signature |= 1 << static_cast<U16>(ComponentType::PLAYER);
    m_signature |= 1 << static_cast<U16>(ComponentType::LOCAL_PLAYER);
}

//----------------------------------------------------------------------------------------------------
bool SightSystemClient::Render()
{
    OPTICK_EVENT();

    LinkingContext& linkingContext = g_gameClient->GetLinkingContext();
    for (const auto& entity : m_entities) {
        NetworkID networkID = linkingContext.GetNetworkID(entity);
        if (networkID >= INVALID_NETWORK_ID) {
            continue;
        }

        std::weak_ptr<TransformComponent> transformComponent = g_gameClient->GetComponentManager().GetComponent<TransformComponent>(entity);
        std::weak_ptr<SightComponent> sightComponent = g_gameClient->GetComponentManager().GetComponent<SightComponent>(entity);
        Draw(transformComponent, sightComponent);
    }

    return true;
}

//----------------------------------------------------------------------------------------------------
void SightSystemClient::OnNotify(const Event& event)
{
    switch (event.eventType) {

    case EventType::SEEN_NEW_ENTITY: {
        OnSeenNewEntity(event.sight.seenEntity);
        break;
    }

    case EventType::SEEN_LOST_ENTITY: {
        OnSeenLostEntity(event.sight.seenEntity);
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
    if (seenEntity >= INVALID_ENTITY) {
        return;
    }

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
    if (seenEntity >= INVALID_ENTITY) {
        return;
    }

    LinkingContext& linkingContext = g_gameClient->GetLinkingContext();
    for (const auto& entity : m_entities) {
        NetworkID networkID = linkingContext.GetNetworkID(entity);
        if (networkID >= INVALID_NETWORK_ID) {
            continue;
        }

        std::weak_ptr<SightComponent> sightComponent = g_gameClient->GetComponentManager().GetComponent<SightComponent>(entity);
        std::vector<Entity>::const_iterator it = std::find(sightComponent.lock()->m_seenEntities.begin(), sightComponent.lock()->m_seenEntities.end(), seenEntity);
        if (it != sightComponent.lock()->m_seenEntities.end()) {
            sightComponent.lock()->m_seenEntities.erase(it);
        }

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
