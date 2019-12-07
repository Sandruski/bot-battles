#include "SpriteComponent.h"

#include "ComponentMemberTypes.h"
#include "Game.h"
#include "MemoryStream.h"
#include "ResourceManager.h"

namespace sand {
//----------------------------------------------------------------------------------------------------
SpriteComponent::SpriteComponent()
    : m_sprite(nullptr)
{
}

//----------------------------------------------------------------------------------------------------
SpriteComponent::~SpriteComponent() { }

//----------------------------------------------------------------------------------------------------
U32 SpriteComponent::Write(OutputMemoryStream& outputStream, U32 dirtyState) const
{
    U32 writtenState = 0;

    if (dirtyState & static_cast<U32>(ComponentMemberType::SPRITE_FILE)) {
        std::string file = m_sprite->GetFile();
        outputStream.Write(file);
        writtenState |= static_cast<U32>(ComponentMemberType::SPRITE_FILE);
    }

    return writtenState;
}

//----------------------------------------------------------------------------------------------------
void SpriteComponent::Read(InputMemoryStream& inputStream, U32 dirtyState)
{
    if (dirtyState & static_cast<U32>(ComponentMemberType::SPRITE_FILE)) {
        std::string file;
        inputStream.Read(file);
        m_sprite = g_game->GetResourceManager().AddResource<SpriteResource>(file.c_str(), TEXTURES_DIR, true);
    }
}
}
