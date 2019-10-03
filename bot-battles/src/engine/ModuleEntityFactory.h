#ifndef __MODULE_ENTITY_FACTORY_H__
#define __MODULE_ENTITY_FACTORY_H__

#include "Module.h"
#include "Memory.h"

#include <unordered_map>
#include <memory>

namespace sand
{

	using EntityID = U64;

	class Entity;

	//----------------------------------------------------------------------------------------------------
	class ModuleEntityFactory : public Module
	{
	public:
		static const char* GetName();
		static EntityID GenerateID();

	public:
		ModuleEntityFactory();
		~ModuleEntityFactory() override;

		bool StartUp() override;
		bool ShutDown() override;

		std::shared_ptr<Entity> AddEntity();

	private:
		std::unordered_map<EntityID, std::shared_ptr<Entity>> m_entities;
	};
}

#endif