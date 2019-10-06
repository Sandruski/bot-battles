#ifndef __COMPONENT_MANAGER_H__
#define __COMPONENT_MANAGER_H__

#include "ComponentDefs.h"
#include "EntityDefs.h"
#include "Memory.h"

#include <memory>
#include <array>
#include <queue>
#include <unordered_map>

namespace sand
{

	struct Component;
	struct ComponentTransform;
	struct ComponentSprite;

	//----------------------------------------------------------------------------------------------------
	template<class T>
	class ComponentArray
	{
	public:
		ComponentArray();
		~ComponentArray();

		ComponentID AddComponent(EntityID entityID);
		bool RemoveComponent(EntityID entityID);
		const T& GetComponent(EntityID entityID);

	private:
		std::array<std::shared_ptr<T>, MAX_ENTITIES> m_components;
		std::queue<ComponentID> m_availableComponentIDs;
		std::unordered_map<ComponentID, ComponentID> m_componentIDsToComponentArrayIDs;
		std::unordered_map<EntityID, ComponentID> m_entityIDsToComponentArrayIDs;
		U32 m_componentsSize;
	};

	//----------------------------------------------------------------------------------------------------
	class ComponentManager
	{
	public:
		ComponentManager();
		~ComponentManager();

		template<class T>
		ComponentID AddComponent(EntityID entityID);
		template<class T>
		bool RemoveComponent(EntityID entityID);
		template<class T>
		const T& GetComponent(EntityID entityID);

	private:
		std::array<ComponentArray<ComponentTransform>, MAX_COMPONENTS> m_components;
	};
}

#endif