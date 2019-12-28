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
        std::string text = m_text.lock()->GetText();
        outputStream.Write(text);
        const SDL_Color& color = m_text.lock()->GetColor();
        outputStream.Write(color);
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
        m_text.lock()->SetFont(rendererComponent.m_font);
        m_text.lock()->SetText(text.c_str());
        m_text.lock()->SetColor(color);
        m_text.lock()->ReLoad();
    }
}
}
