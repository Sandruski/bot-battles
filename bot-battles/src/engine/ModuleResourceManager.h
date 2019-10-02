#ifndef __MODULE_RESOURCE_MANAGER_H__
#define __MODULE_RESOURCE_MANAGER_H__

#include "Module.h"

#include "Memory.h"
#include "Utils.h"
#include "Log.h"

#include <cassert>
#include <unordered_map>
#include <memory>

namespace sand
{

	class Resource;

	//----------------------------------------------------------------------------------------------------
	class ModuleResourceManager : public Module
	{
	public:
		ModuleResourceManager();
		~ModuleResourceManager() override;

		bool StartUp() override;
		bool ShutDown() override;

		const char* GetName() const override;

		template<class T>
		std::shared_ptr<T> Add(const char* file, const char* dir);
		template<class T>
		U32 Remove(U64 uuid);
		void RemoveAll();

		template<class T>
		std::shared_ptr<T> Get(U64 uuid);

	private:
		template<class T>
		std::shared_ptr<T> Get(const char* file, const char* dir);

	private:
		std::unordered_map<U64, std::shared_ptr<Resource>> m_resources;
	};

	//----------------------------------------------------------------------------------------------------
	template<class T>
	inline std::shared_ptr<T> ModuleResourceManager::Add(const char* file, const char* dir)
	{
		assert(file != nullptr && dir != nullptr);

		std::shared_ptr<T> resource = Get<T>(file, dir);
		if (resource != nullptr)
		{
			resource->IncreaseReferences();
			return resource;
		}

		U64 uuid = 0;
		resource = std::make_shared<T>(uuid, file, dir);

		const bool isLoaded = resource->Load();
		if (!isLoaded)
		{
			LOG("Resource %s%s could not be loaded", dir, file);
			return nullptr;
		}

		auto inserted = m_resources.insert(std::make_pair(uuid, resource));
		if (inserted.second)
		{
			LOG("Resource %s%s could not be inserted", dir, file);
			return std::static_pointer_cast<T>(inserted.first->second);
		}

		return resource;
	}

	//----------------------------------------------------------------------------------------------------
	template<class T>
	inline U32 ModuleResourceManager::Remove(U64 uuid)
	{
		std::shared_ptr<T> resource = Get(uuid);
		if (resource == nullptr)
		{
			return -1;
		}

		resource->DecreaseReferences();
		if (!resource->HasReferences())
		{
			resource->UnLoad();
			m_resources.erase(uuid);
			
			return 0;
		}

		return resource->GetReferences();
	}

	//----------------------------------------------------------------------------------------------------
	template<class T>
	inline std::shared_ptr<T> ModuleResourceManager::Get(U64 uuid)
	{
		auto it = m_resources.find(uuid);
		if (it != m_resources.end())
		{
			return std::static_pointer_cast<T>((*it).second);
		}

		return nullptr;
	}

	//----------------------------------------------------------------------------------------------------
	template<class T>
	inline std::shared_ptr<T> ModuleResourceManager::Get(const char* file, const char* dir)
	{
		assert(file != nullptr && dir != nullptr);

		for (auto resource : m_resources)
		{
			if (COMPARE_STRINGS(resource.second->GetFile(), file)
				&& COMPARE_STRINGS(resource.second->GetDir(), dir))
			{
				return std::static_pointer_cast<T>(resource.second);
			}
		}

		return nullptr;
	}
}

#endif
