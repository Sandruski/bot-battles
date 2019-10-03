#ifndef __ENTITY_H
#define __ENTITY_H

#include "Component.h"
#include "ComponentTypes.h"

#include "Engine.h"
#include "ModuleEntityFactory.h"

#include <vector>
#include <memory>

namespace sand
{

	//----------------------------------------------------------------------------------------------------
	class Entity
	{
	public:
		Entity();
		~Entity();

		template <class T>
		std::shared_ptr<T> AddComponent();

		template <class T>
		std::shared_ptr<T> GetComponent();

	private:
		std::shared_ptr<Component> m_components[ComponentType::COMPONENT_COUNT];
	};

	template<class T>
	inline std::shared_ptr<T> Entity::AddComponent()
	{
		static_assert(std::is_base_of<Component, T>::value, "T is not derived from Component");

		ComponentType type = T::GetType();
		std::shared_ptr<T> component = std::make_shared<T>(this);

		m_components[type] = component;

		return component;
	}

	template<class T>
	inline std::shared_ptr<T> Entity::GetComponent()
	{
		static_assert(std::is_base_of<Component, T>::value, "T is not derived from Component");

		return std::shared_ptr<T>();
	}
}

#endif