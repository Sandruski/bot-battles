#ifndef __SPRITE_COMPONENT_H__
#define __SPRITE_COMPONENT_H__

#include "ComponentMemberTypes.h"
#include "ComponentTypes.h"
#include "Game.h"
#include "MemoryStream.h"
#include "NetComponent.h"
#include "ResourceManager.h"
#include "ResourceTexture.h"

namespace sand {

//----------------------------------------------------------------------------------------------------
struct SpriteComponent : public NetComponent {

    static ComponentType GetType() { return ComponentType::SPRITE; }
    static SpriteComponent* Instantiate() { return new SpriteComponent(); }

    SpriteComponent()
        : m_texture(nullptr)
    {
    }
    ~SpriteComponent() { }

    U32 Write(OutputMemoryStream& outputStream, U32 dirtyState) const override
    {
        U32 writtenState = 0;

        if (dirtyState & static_cast<U32>(ComponentMemberType::SPRITE_FILE)) {
            std::string file = m_texture->GetFile();
            outputStream.Write(file);
            writtenState |= static_cast<U32>(ComponentMemberType::SPRITE_FILE);
        }

        return writtenState;
    }

    void Read(InputMemoryStream& inputStream, U32 dirtyState) override
    {
        if (dirtyState & static_cast<U32>(ComponentMemberType::SPRITE_FILE)) {
            std::string file;
            inputStream.Read(file);
            m_texture = g_game->GetResourceManager().AddResource<ResourceTexture>(file.c_str(), "../../data/textures/");
        }
    }

    std::shared_ptr<ResourceTexture> m_texture;
};
}

#endif
