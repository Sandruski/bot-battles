#ifndef __FONT_RESOURCE_H__
#define __FONT_RESOURCE_H__

#include "Resource.h"

namespace sand {
//----------------------------------------------------------------------------------------------------
class FontResource : public Resource {
public:
    FontResource(U32 id, const char* dir, const char* file);

    bool Load() override;
    bool UnLoad() override;

    bool ReLoad();

    TTF_Font* GetFont() const;
    void SetSize(U16 size);
    U16 GetSize() const;

private:
    TTF_Font* m_font;
    U16 m_size;
};
}

#endif
