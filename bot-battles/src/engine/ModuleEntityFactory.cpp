#include "ModuleEntityFactory.h"

#include "Entity.h"
#include "ComponentTransform.h"

namespace sand
{

	//----------------------------------------------------------------------------------------------------
	const char* ModuleEntityFactory::GetName()
	{
		return "EntityFactory";
	}

	//----------------------------------------------------------------------------------------------------
	EntityID ModuleEntityFactory::GenerateID()
	{
		static EntityID currentID = 0;
		++currentID;
		return currentID;
	}

	//----------------------------------------------------------------------------------------------------
	ModuleEntityFactory::ModuleEntityFactory() : Module(true)
	{
	}

	//----------------------------------------------------------------------------------------------------
	ModuleEntityFactory::~ModuleEntityFactory()
	{
	}

	//----------------------------------------------------------------------------------------------------
	bool ModuleEntityFactory::StartUp()
	{
		return true;
	}

	//----------------------------------------------------------------------------------------------------
	bool ModuleEntityFactory::ShutDown()
	{
		return true;
	}

	//----------------------------------------------------------------------------------------------------
	std::shared_ptr<Entity> ModuleEntityFactory::AddEntity()
	{
		std::shared_ptr<Entity> entity = std::make_shared<Entity>();
		entity->AddComponent<ComponentTransform>();

		EntityID id = GenerateID(); // TODO
		m_entities.insert(std::make_pair(id, entity));

		return entity;
	}
}