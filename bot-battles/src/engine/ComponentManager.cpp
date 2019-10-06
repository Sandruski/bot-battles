#include "ComponentManager.h"

#include "Component.h"
#include "ComponentTransform.h"
#include "ComponentSprite.h"
#include "ComponentRenderer.h"

#include "Game.h"
#include "EntityManager.h"
#include "Entity.h"

#include "Log.h"

#include <cassert>

namespace sand
{

	//----------------------------------------------------------------------------------------------------
	template<class T>
	ComponentArray<T>::ComponentArray<T>() : m_componentsSize(0)
	{
		memset(&m_components, INVALID_ENTITY, MAX_ENTITIES);

		for (ComponentID i = 0; i < MAX_ENTITIES; ++i)
		{
			m_availableComponentIDs.push(i);
		}
	}

	//----------------------------------------------------------------------------------------------------
	template<class T>
	ComponentArray<T>::~ComponentArray<T>()
	{
		while (!m_availableComponentIDs.empty())
		{
			m_availableComponentIDs.pop();
		}
	}

	//----------------------------------------------------------------------------------------------------
	template<class T>
	ComponentID ComponentArray<T>::AddComponent(EntityID entityID)
	{
		auto it = m_entityIDsToComponentArrayIDs.find(entityID);
		if (it != m_entityIDsToComponentArrayIDs.end())
		{
			LOG("The entity %u already has the component!", entityID);
			return it->second;
		}

		ComponentID componentID = m_availableComponentIDs.front();
		m_availableComponentIDs.pop();

		ComponentID componentArrayID = m_componentsSize;
		m_components[componentArrayID] = std::make_shared<T>(componentID);
		++m_componentsSize;

		m_entityIDsToComponentArrayIDs[entityID] = componentArrayID;

		return componentID;
	}

	//----------------------------------------------------------------------------------------------------
	template<class T>
	bool ComponentArray<T>::RemoveComponent(EntityID entityID)
	{
		auto it = m_entityIDsToComponentArrayIDs.find(entityID);
		if (it == m_entityIDsToComponentArrayIDs.end())
		{
			LOG("The entity %u does not have the component!");
			return false;
		}

		ComponentID componentArrayID = it->second;
		ComponentID lastComponentArrayID = m_componentsSize - 1;
		std::shared_ptr<T> lastComponent = m_components[lastComponentArrayID];
		std::shared_ptr<T> component = m_components[componentArrayID];
		ComponentID componentID = component->m_id;
		m_components[componentArrayID] = lastComponent;
		m_components[lastComponentArrayID] = nullptr;
		--m_componentsSize;

		m_entityIDsToComponentArrayIDs.erase(entityID);

		ComponentID lastComponentID = lastComponent->m_id;
		it = std::find_if(m_entityIDsToComponentArrayIDs.begin(), 
			m_entityIDsToComponentArrayIDs.end(), 
			[lastComponentArrayID](const auto& value)
			{
				return value.second == lastComponentArrayID;
			});
		m_entityIDsToComponentArrayIDs[it->first] = componentArrayID;

		m_availableComponentIDs.push(componentID);

		return true;
	}

	//----------------------------------------------------------------------------------------------------
	template<class T>
	const T& ComponentArray<T>::GetComponent(EntityID entityID)
	{
		ComponentID componentArrayID = m_entityIDsToComponentArrayIDs.at(entityID);

		return m_components[componentArrayID];
	}

	//----------------------------------------------------------------------------------------------------
	ComponentManager::ComponentManager()
	{
		m_transform = std::make_unique<ComponentArray<ComponentTransform>>();
		m_sprite = std::make_unique<ComponentArray<ComponentSprite>>();

		m_renderer = std::make_unique<ComponentRenderer>(0);
	}

	//----------------------------------------------------------------------------------------------------
	ComponentManager::~ComponentManager()
	{
	}

	//----------------------------------------------------------------------------------------------------
	template ComponentID ComponentManager::AddComponent<ComponentTransform>(EntityID entityID);
	template ComponentID ComponentManager::AddComponent<ComponentSprite>(EntityID entityID);

	template<class T>
	ComponentID ComponentManager::AddComponent(EntityID entityID)
	{
		static_assert(std::is_base_of<Component, T>::value, "T is not derived from Component");
		assert(entityID < INVALID_ENTITY);

		switch (T::GetType())
		{
		case ComponentType::TRANSFORM:
		{
			return m_transform->AddComponent(entityID);
		}

		case ComponentType::SPRITE:
		{
			return m_sprite->AddComponent(entityID);
		}

		default:
			break;
		}

		return INVALID_COMPONENT;
	}

	//----------------------------------------------------------------------------------------------------
	template bool ComponentManager::RemoveComponent<ComponentTransform>(EntityID entityID);
	template bool ComponentManager::RemoveComponent<ComponentSprite>(EntityID entityID);

	template<class T>
	bool ComponentManager::RemoveComponent(EntityID entityID)
	{
		static_assert(std::is_base_of<Component, T>::value, "T is not derived from Component");
		assert(entityID < INVALID_ENTITY);

		switch (T::GetType())
		{
		case ComponentType::TRANSFORM:
		{
			return m_transform->RemoveComponent(entityID);
		}

		case ComponentType::SPRITE:
		{
			return m_sprite->RemoveComponent(entityID);
		}

		default:
			break;
		}

		return false;
	}

	//----------------------------------------------------------------------------------------------------
	template<class T>
	const T& ComponentManager::GetComponent(EntityID entityID)
	{
		static_assert(std::is_base_of<Component, T>::value, "T is not derived from Component");
		assert(entityID < INVALID_ENTITY);

		switch (T::GetType())
		{
		case ComponentType::TRANSFORM:
		{
			return m_transform->GetComponent(entityID);
		}

		case ComponentType::SPRITE:
		{
			return m_sprite->GetComponent(entityID);
		}

		default:
			break;
		}

		return INVALID_COMPONENT;
	}

	//----------------------------------------------------------------------------------------------------
	template ComponentRenderer& ComponentManager::GetSingletonComponent<ComponentRenderer>();

	template<class T>
	T& ComponentManager::GetSingletonComponent()
	{
		static_assert(std::is_base_of<Component, T>::value, "T is not derived from Component");

		switch (T::GetType())
		{
		case ComponentType::RENDERER:
		{
			return *m_renderer;
		}

		default:
			break;
		}

		return *m_renderer;
	}
}
