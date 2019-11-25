#ifndef __FONT_IMPORTER_H__
#define __FONT_IMPORTER_H__

namespace sand
{
	//----------------------------------------------------------------------------------------------------
	class FontImporter
	{
	public:
		FontImporter();
		~FontImporter();

		bool StartUp() const;
		bool ShutDown() const;

		TTF_Font* Load(const char* path, U16 size) const;
		void UnLoad(TTF_Font*& font) const;
	};
}

#endif
