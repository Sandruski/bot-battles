#include "Resource.h"

#include "Memory.h"

#include <cstring>
#include <cstdio>
#include <cassert>

namespace sand
{

	//----------------------------------------------------------------------------------------------------
	Resource::Resource(const char* file, const char* dir) :
		m_uuid(0),
		m_references(0)
	{
		assert(file != nullptr && dir != nullptr);

		size_t fileSize = strlen(file);
		m_file = new char[fileSize + 1];
		memcpy(&m_file, file, (fileSize + 1) * sizeof(char));

		size_t dirSize = strlen(dir);
		m_dir = new char[dirSize + 1];
		memcpy(&m_dir, dir, (dirSize + 1) * sizeof(char));

		size_t pathSize = fileSize + dirSize;
		m_path = new char[pathSize + 1];
		sprintf_s(m_path, pathSize + 1, "%s%s", dir, file);
	}

	//----------------------------------------------------------------------------------------------------
	Resource::~Resource()
	{
		SAFE_DELETE_POINTER(m_file);
		SAFE_DELETE_POINTER(m_dir);
		SAFE_DELETE_POINTER(m_path);
	}

	//----------------------------------------------------------------------------------------------------
	const char* Resource::GetFile() const
	{
		return m_file;
	}

	//----------------------------------------------------------------------------------------------------
	const char* Resource::GetDir() const
	{
		return m_dir;
	}

	//----------------------------------------------------------------------------------------------------
	const char* Resource::GetPath() const
	{
		return m_path;
	}

	//----------------------------------------------------------------------------------------------------
	U64 Resource::GetUUID() const
	{
		return m_uuid;
	}

	U32 Resource::GetReferences() const
	{
		return m_references;
	}

	//----------------------------------------------------------------------------------------------------
	bool Resource::HasReferences() const
	{
		return m_references > 0;
	}

	//----------------------------------------------------------------------------------------------------
	U32 Resource::IncreaseReferences()
	{
		++m_references;
		return m_references;
	}

	//----------------------------------------------------------------------------------------------------
	U32 Resource::DecreaseReferences()
	{
		--m_references;
		return m_references;
	}
}
