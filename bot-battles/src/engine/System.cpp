#include "System.h"

#include <cassert>

namespace sand
{

	//----------------------------------------------------------------------------------------------------
	System::System()
	{
	}

	//----------------------------------------------------------------------------------------------------
	System::~System()
	{
	}

	//----------------------------------------------------------------------------------------------------
	bool System::RegisterEntity(const std::shared_ptr<Entity>& entity)
	{
		auto it = std::find(m_entities.begin(), m_entities.end(), entity);
		assert(it == m_entities.end());

		m_entities.push_back(entity);

		return true;
	}

	//----------------------------------------------------------------------------------------------------
	bool System::DeregisterEntity(const std::shared_ptr<Entity>& entity)
	{
		auto it = std::find(m_entities.begin(), m_entities.end(), entity);
		assert(it != m_entities.end());

		m_entities.erase(it);

		return true;
	}
}