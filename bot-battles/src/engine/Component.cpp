#include "Component.h"

namespace sand
{

	//----------------------------------------------------------------------------------------------------
	Component::Component(Entity* owner) : 
		m_owner(owner),
		m_isEnabled(false)
	{
	}

	//----------------------------------------------------------------------------------------------------
	Component::~Component()
	{
	}
}