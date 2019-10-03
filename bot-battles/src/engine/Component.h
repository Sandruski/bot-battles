#ifndef __COMPONENT_H__
#define __COMPONENT_H__

#include <memory>

namespace sand
{

	class Entity;

	//----------------------------------------------------------------------------------------------------
	class Component
	{
	public:
		Component(Entity* owner);
		virtual ~Component();

		std::shared_ptr<Entity> GetOwner() const
		{
			return m_owner;
		}

		bool IsEnabled() const
		{
			return m_isEnabled;
		}

	private:
		std::shared_ptr<Entity> m_owner;
		bool m_isEnabled;
	};
}

#endif