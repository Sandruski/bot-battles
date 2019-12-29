#include "TextComponent.h"

#include "ComponentMemberTypes.h"
#include "Game.h"
#include "MemoryStream.h"
#include "RendererComponent.h"
#include "ResourceManager.h"
#include "TextResource.h"

namespace sand {
//----------------------------------------------------------------------------------------------------
TextComponent::TextComponent()
    : m_text()
{
}

//----------------------------------------------------------------------------------------------------
U32 TextComponent::Write(OutputMemoryStream& outputStream, U32 dirtyState) const
{
    U32 writtenState = 0;

    if (dirtyState & static_cast<U32>(ComponentMemberType::TEXT_STUFF)) {
        outputStream.Write(m_text.lock()->m_text);
        outputStream.Write(m_text.lock()->m_color);
        writtenState |= static_cast<U32>(ComponentMemberType::TEXT_STUFF);
    }

    return writtenState;
}

//----------------------------------------------------------------------------------------------------
void TextComponent::Read(InputMemoryStream& inputStream, U32 dirtyState)
{
    if (dirtyState & static_cast<U32>(ComponentMemberType::TEXT_STUFF)) {
        std::string text;
        inputStream.Read(text);
        SDL_Color color;
        inputStream.Read(color);
        m_text = g_game->GetResourceManager().AddResource<TextResource>("", "", false);
        RendererComponent& rendererComponent = g_game->GetRendererComponent();
        m_text.lock()->m_font = rendererComponent.m_font;
        m_text.lock()->m_text = text.c_str();
        m_text.lock()->m_color = color;
        m_text.lock()->ReLoad();
    }
}
}
