#ifndef __TEXT_RESOURCE_H__
#define __TEXT_RESOURCE_H__

#include "FontResource.h"
#include "Resource.h"

#include "Vec2I.h"

namespace sand
{
	//----------------------------------------------------------------------------------------------------
	class TextResource : public Resource
	{
	public:
		TextResource(U32 id, const char* dir, const char* file);
		~TextResource() override;

		bool Load() override;
		bool UnLoad() override;

		SDL_Texture* GetTexture() const
		{
			return m_texture;
		}

		void SetFont(std::shared_ptr<FontResource> font);

		void SetText(const char* text);

		const char* GetText() const
		{
			return m_text.c_str();
		}

		void SetColor(const SDL_Color& color);

		const SDL_Color& GetColor() const
		{
			return m_color;
		}

		U32 GetWidth() const
		{
			return m_size.x;
		}

		U32 GetHeight() const
		{
			return m_size.y;
		}

	private:
		bool ReLoad();

	private:
		SDL_Texture* m_texture;
		std::shared_ptr<FontResource> m_font;
		std::string m_text;
		SDL_Color m_color;
		Vec2I m_size;
	};
}

#endif
