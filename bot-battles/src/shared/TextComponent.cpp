#include "TextComponent.h"

#include "ComponentMemberTypes.h"
#include "Game.h"
#include "MemoryStream.h"
#include "ResourceManager.h"
#include "SingletonRendererComponent.h"

namespace sand
{
	//----------------------------------------------------------------------------------------------------
	TextComponent::TextComponent(): m_text(nullptr)
	{
	}

	//----------------------------------------------------------------------------------------------------
	TextComponent::~TextComponent()
	{
	}

	//----------------------------------------------------------------------------------------------------
	U32 TextComponent::Write(OutputMemoryStream& outputStream, U32 dirtyState) const
	{
		U32 writtenState = 0;

		if (dirtyState & static_cast<U32>(ComponentMemberType::TEXT_STUFF)) {
			std::string text = m_text->GetText();
			outputStream.Write(text);
			const SDL_Color& color = m_text->GetColor();
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
			m_text = g_game->GetResourceManager().AddResource<TextResource>(nullptr, nullptr);
			std::shared_ptr<SingletonRendererComponent> singletonRenderer = g_game->GetSingletonRendererComponent();
			m_text->SetFont(singletonRenderer->m_font);
			m_text->SetText(text.c_str());
			m_text->SetColor(color);
		}
	}
}
