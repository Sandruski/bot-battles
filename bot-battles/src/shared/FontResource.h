#ifndef __FONT_RESOURCE_H__
#define __FONT_RESOURCE_H__

#include "Resource.h"

namespace sand
{
	//----------------------------------------------------------------------------------------------------
	class FontResource : public Resource
	{
	public:
		FontResource(U32 id, const char* dir, const char* file);
		~FontResource() override;

		bool Load() override;
		bool UnLoad() override;

		TTF_Font* GetFont() const
		{
			return m_font;
		}

		void SetSize(U16 size);

		U16 GetSize() const
		{
			return m_size;
		}

	private:
		bool ReLoad();

	private:
		TTF_Font* m_font;
		U16 m_size;
	};
}

#endif
