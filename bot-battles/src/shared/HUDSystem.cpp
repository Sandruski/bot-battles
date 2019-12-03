#include "HUDSystem.h"

#include "Game.h"
#include "ComponentManager.h"
#include "SingletonRendererComponent.h"
#include "TransformComponent.h"
#include "TextComponent.h"

namespace sand
{

	//----------------------------------------------------------------------------------------------------
	HUDSystem::HUDSystem()
	{
		m_signature |= 1 << static_cast<U16>(ComponentType::TRANSFORM);
		m_signature |= 1 << static_cast<U16>(ComponentType::TEXT);
	}

	//----------------------------------------------------------------------------------------------------
	HUDSystem::~HUDSystem()
	{

	}

	//----------------------------------------------------------------------------------------------------
	bool HUDSystem::Render()
	{
		std::shared_ptr<SingletonRendererComponent> singletonRenderer = g_game->GetSingletonRendererComponent();

		for (auto& entity : m_entities) {

			std::shared_ptr<TransformComponent> transformComponent = g_game->GetComponentManager().GetComponent<TransformComponent>(entity);
			std::shared_ptr<TextComponent> textComponent = g_game->GetComponentManager().GetComponent<TextComponent>(entity);

			SDL_Rect renderQuad = {
				static_cast<I32>(transformComponent->m_position.x),
				static_cast<I32>(transformComponent->m_position.y),
				static_cast<I32>(textComponent->m_text->GetWidth()),
				static_cast<I32>(textComponent->m_text->GetHeight())
			};

			SDL_RenderCopy(singletonRenderer->m_renderer, textComponent->m_text->GetTexture(), nullptr, &renderQuad);

			return true;
		}
	}
}