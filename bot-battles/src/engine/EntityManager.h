#ifndef __ENTITY_MANAGER_H__
#define __ENTITY_MANAGER_H__

#include "EntityDefs.h"
#include "Memory.h"

#include <memory>
#include <array>
#include <queue>
#include <unordered_map>

namespace sand
{

	struct Entity;

	//----------------------------------------------------------------------------------------------------
	class EntityManager
	{
	public:
		EntityManager();
		~EntityManager();

		EntityID AddEntity();
		bool RemoveEntity(EntityID entityID);
		const std::shared_ptr<Entity>& GetEntity(EntityID entityID);

	private:
		std::array<std::shared_ptr<Entity>, MAX_ENTITIES> m_entities;
		std::queue<EntityID> m_availableEntityIDs;
		std::unordered_map<EntityID, EntityID> m_entityIDsToEntityArrayIDs;
		U32 m_entitiesSize;
	};
}

#endif