#ifndef __RESOURCE_H__
#define __RESOURCE_H__

#include "Memory.h"

namespace sand
{

	//----------------------------------------------------------------------------------------------------
	class Resource
	{
	public:
		Resource(U32 id, const char* file, const char* dir);
		virtual ~Resource();

		const char* GetFile() const;
		const char* GetDir() const;
		const char* GetPath() const;

		U32 GetID() const;
		U32 GetReferences() const;

		bool HasReferences() const;

		U32 IncreaseReferences();
		U32 DecreaseReferences();

		virtual bool Load() = 0;
		virtual bool UnLoad() = 0;

	private:
		char* m_file;
		char* m_dir;
		char* m_path;

		U32 m_id;
		U32 m_references;
	};
}

#endif
