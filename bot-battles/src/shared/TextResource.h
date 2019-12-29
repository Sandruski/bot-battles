#ifndef __TEXT_RESOURCE_H__
#define __TEXT_RESOURCE_H__

#include "FontResource.h"
#include "Resource.h"

#include "Vec2I.h"

namespace sand {

//----------------------------------------------------------------------------------------------------
class TextResource : public Resource {
public:
    TextResource(U32 id, const char* dir, const char* file);

    bool Load() override;
    bool UnLoad() override;

    bool ReLoad();

    SDL_Texture* GetTexture() const;
    U32 GetWidth() const;
    U32 GetHeight() const;

public:
    std::weak_ptr<FontResource> m_font;
    std::string m_text;
    SDL_Color m_color;

private:
    SDL_Texture* m_texture;
    Vec2I m_size;
};
}

#endif
