#include "SpriteComponent.h"

#include "ClientComponent.h"
#include "ComponentManager.h"
#include "ComponentMemberTypes.h"
#include "GameClient.h"
#include "ReplicationManagerClient.h"
#include "SpriteResource.h"

namespace sand {

//----------------------------------------------------------------------------------------------------
void SpriteComponent::Read(InputMemoryStream& inputStream, U32 dirtyState, U32 /*frame*/, ReplicationActionType /*replicationActionType*/, Entity entity)
{
    if (dirtyState & static_cast<U32>(ComponentMemberType::SPRITE_FILE)) {
        std::string file;
        inputStream.Read(file);
        m_spriteResource = g_game->GetResourceManager().AddResource<SpriteResource>(file.c_str(), TEXTURES_DIR, true);
    }
    if (dirtyState & static_cast<U32>(ComponentMemberType::SPRITE_SPRITE_NAME_TO_TEXTURE_COORDS)) {
        inputStream.Read(m_spriteNameToTextureCoords);

        Event newComponentEvent;
        newComponentEvent.eventType = EventType::COMPONENT_MEMBER_CHANGED;
        newComponentEvent.component.dirtyState = static_cast<U32>(ComponentMemberType::SPRITE_SPRITE_NAME_TO_TEXTURE_COORDS);
        newComponentEvent.component.entity = entity;
        ClientComponent& clientComponent = g_gameClient->GetClientComponent();
        clientComponent.m_replicationManager.NotifyEvent(newComponentEvent);
    }
    if (dirtyState & static_cast<U32>(ComponentMemberType::SPRITE_SPRITE_NAME)) {
        inputStream.Read(m_spriteName);
    }
    if (dirtyState & static_cast<U32>(ComponentMemberType::SPRITE_VISIBLE)) {
        inputStream.Read(m_isVisible);
    }
}
}
