#include "EntityManager.h"
#include "Entity.h"

#include "Game.h"
#include "ComponentManager.h"
#include "ComponentTransform.h"

#include "Log.h"

#include <cassert>

namespace sand
{

	//----------------------------------------------------------------------------------------------------
	EntityManager::EntityManager() : m_entitiesSize(0)
	{
		memset(&m_entities, INVALID_ENTITY, MAX_ENTITIES);

		for (EntityID i = 0; i < MAX_ENTITIES; ++i)
		{
			m_availableEntityIDs.push(i);
		}
	}

	//----------------------------------------------------------------------------------------------------
	EntityManager::~EntityManager()
	{
		while (!m_availableEntityIDs.empty())
		{
			m_availableEntityIDs.pop();
		}

		m_entityIDsToEntityArrayIDs.clear();
	}

	//----------------------------------------------------------------------------------------------------
	EntityID EntityManager::AddEntity()
	{
		EntityID entityID = m_availableEntityIDs.front();
		m_availableEntityIDs.pop();

		EntityID arrayEntityID = m_entitiesSize;
		m_entities[arrayEntityID] = std::make_shared<Entity>(entityID);
		++m_entitiesSize;

		m_entityIDsToEntityArrayIDs[entityID] = arrayEntityID;

		return entityID;
	}

	//----------------------------------------------------------------------------------------------------
	bool EntityManager::RemoveEntity(EntityID entityID)
	{
		assert(entityID < INVALID_ENTITY);

		auto it = m_entityIDsToEntityArrayIDs.find(entityID);
		if (it == m_entityIDsToEntityArrayIDs.end())
		{
			LOG("The entity %u does not exist!");
			return false;
		}

		// TODO: remove all of its components

		EntityID arrayEntityID = it->second;
		EntityID lastEntityArrayID = m_entitiesSize - 1;
		std::shared_ptr<Entity> lastEntity = m_entities[lastEntityArrayID];
		m_entities[arrayEntityID] = lastEntity;
		m_entities[lastEntityArrayID] = nullptr;
		--m_entitiesSize;

		m_entityIDsToEntityArrayIDs.erase(entityID);

		EntityID lastEntityID = lastEntity->m_id;
		m_entityIDsToEntityArrayIDs[lastEntityID] = arrayEntityID;
		
		m_availableEntityIDs.push(entityID);

		return true;
	}

	//----------------------------------------------------------------------------------------------------
	const std::shared_ptr<Entity>& EntityManager::GetEntity(EntityID entityID)
	{
		EntityID entityArrayID = m_entityIDsToEntityArrayIDs.at(entityID);
		
		return m_entities[entityArrayID];
	}
}
