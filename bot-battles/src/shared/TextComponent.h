#ifndef __TEXT_COMPONENT_H__
#define __TEXT_COMPONENT_H__

#include "ComponentMemberTypes.h"
#include "ComponentTypes.h"
#include "Game.h"
#include "MemoryStream.h"
#include "NetComponent.h"
#include "ResourceManager.h"
#include "SingletonRendererComponent.h"
#include "TextResource.h"

namespace sand {

	//----------------------------------------------------------------------------------------------------
	struct TextComponent : public NetComponent {

		static ComponentType GetType() { return ComponentType::TEXT; }
		static TextComponent* Instantiate() { return new TextComponent(); }

		TextComponent()
			: m_text(nullptr)
		{
		}
		~TextComponent() { }

		U32 Write(OutputMemoryStream& outputStream, U32 dirtyState) const override
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

		void Read(InputMemoryStream& inputStream, U32 dirtyState) override
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

		std::shared_ptr<TextResource> m_text;
	};
}

#endif
