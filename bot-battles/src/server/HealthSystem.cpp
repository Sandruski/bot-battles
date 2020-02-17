#include "HealthSystem.h"

#include "HealthComponent.h"
#include "GameServer.h"
#include "ComponentManager.h"
#include "SpriteComponent.h"

namespace sand {

	//----------------------------------------------------------------------------------------------------
	HealthSystem::HealthSystem()
	{
		m_signature |= 1 << static_cast<U16>(ComponentType::HEALTH);
		m_signature |= 1 << static_cast<U16>(ComponentType::SPRITE);
	}

	//----------------------------------------------------------------------------------------------------
	bool HealthSystem::Update()
	{
		for (auto& entity : m_entities) {
			std::weak_ptr<HealthComponent> healthComponent = g_gameServer->GetComponentManager().GetComponent<HealthComponent>(entity);
			if (healthComponent.lock()->m_health <= 0)
			{
				healthComponent.lock()->m_health = 0;
				
				std::weak_ptr<SpriteComponent> spriteComponent = g_gameServer->GetComponentManager().GetComponent<SpriteComponent>(entity);
				spriteComponent.lock()->m_isEnabled = false;
			}
		}

		return true;
	}
}