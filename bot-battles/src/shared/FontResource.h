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

public:
    U16 m_size;

private:
    TTF_Font* m_font;
};
}

#endif
