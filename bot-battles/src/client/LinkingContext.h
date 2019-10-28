#ifndef __LINKING_CONTEXT_H__
#define __LINKING_CONTEXT_H__

#include "NetworkDefs.h"
#include "EntityDefs.h"

namespace sand
{

	//----------------------------------------------------------------------------------------------------
	class LinkingContext
	{
	public:
		LinkingContext();
		~LinkingContext();

		NetworkID GetNetworkID(Entity entity) const;
		Entity GetEntity(NetworkID networkID) const;

		bool AddEntity(Entity entity);
		bool RemoveEntity(Entity entity);

	private:
		std::unordered_map<NetworkID, Entity> m_networkIDToEntity;
		std::unordered_map<Entity, NetworkID> m_entityToNetworkID;
		std::queue<Entity> m_availableNetworkIDs;
	};
}

#endif
