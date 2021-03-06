#include "SpriteComponent.h"

#include "ClientComponent.h"
#include "ComponentManager.h"
#include "ComponentMemberTypes.h"
#include "GameClient.h"
#include "ReplicationManagerClient.h"
#include "ResourceManager.h"
#include "SpriteResource.h"

namespace sand {

//----------------------------------------------------------------------------------------------------
void SpriteComponent::Read(InputMemoryStream& inputStream, U64 dirtyState, U32 /*frame*/, ReplicationActionType /*replicationActionType*/, Entity entity)
{
    U64 characterDirtyState = 0;

    if (dirtyState & static_cast<U64>(ComponentMemberType::SPRITE_FILE)) {
        std::string file;
        inputStream.Read(file);
        if (!m_spriteResource.expired()) {
            g_game->GetResourceManager().RemoveResource<SpriteResource>(m_spriteResource.lock()->GetID());
        }
        m_spriteResource = g_gameClient->GetResourceManager().AddResource<SpriteResource>(file.c_str(), TEXTURES_DIR, true);
    }
    if (dirtyState & static_cast<U64>(ComponentMemberType::SPRITE_SPRITE_NAME_TO_TEXTURE_COORDS)) {
        inputStream.Read(m_spriteNameToTextureCoords);
        characterDirtyState |= static_cast<U64>(ComponentMemberType::SPRITE_SPRITE_NAME_TO_TEXTURE_COORDS);
    }
    if (dirtyState & static_cast<U64>(ComponentMemberType::SPRITE_SPRITE_NAME)) {
        inputStream.Read(m_spriteName);
        characterDirtyState |= static_cast<U64>(ComponentMemberType::SPRITE_SPRITE_NAME);
    }
    if (dirtyState & static_cast<U64>(ComponentMemberType::SPRITE_FLIPPED_HORIZONTALLY)) {
        inputStream.Read(m_isFlippedHorizontally);
        characterDirtyState |= static_cast<U64>(ComponentMemberType::SPRITE_FLIPPED_HORIZONTALLY);
    }
    if (dirtyState & static_cast<U64>(ComponentMemberType::SPRITE_FLIPPED_VERTICALLY)) {
        inputStream.Read(m_isFlippedVertically);
        characterDirtyState |= static_cast<U64>(ComponentMemberType::SPRITE_FLIPPED_VERTICALLY);
    }

    if (characterDirtyState > 0) {
        Event newComponentEvent;
        newComponentEvent.eventType = EventType::COMPONENT_MEMBER_CHANGED;
        newComponentEvent.component.dirtyState = characterDirtyState;
        newComponentEvent.component.entity = entity;
        std::weak_ptr<ClientComponent> clientComponent = g_gameClient->GetClientComponent();
        clientComponent.lock()->m_replicationManager.NotifyEvent(newComponentEvent);
    }
}
}
