#ifndef __SYSTEM_H__
#define __SYSTEM_H__

#include "Memory.h"
#include "ComponentDefs.h"

#include <bitset>
#include <vector>

namespace sand
{

	struct Entity;

	//----------------------------------------------------------------------------------------------------
	class System
	{
	public:
		System();
		virtual ~System();

		virtual bool StartUp() 
		{ 
			return true; 
		}

		virtual bool Update(F32 dt) 
		{
			return true;
		}

		virtual bool LateUpdate(F32 dt)
		{
			return true;
		}

		virtual bool Render()
		{
			return true;
		}

		virtual bool ShutDown()
		{
			return true;
		}

		bool RegisterEntity(const std::shared_ptr<Entity>& entity);
		bool DeregisterEntity(const std::shared_ptr<Entity>& entity);

		//void NotifyComponent(Component*);

	protected:
		std::bitset<MAX_COMPONENTS> m_signature;
		std::vector<std::shared_ptr<Entity>> m_entities;
	};
}

#endif