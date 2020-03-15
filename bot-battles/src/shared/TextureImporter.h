#ifndef __TEXTURE_IMPORTER_H__
#define __TEXTURE_IMPORTER_H__

namespace sand {

//----------------------------------------------------------------------------------------------------
class TextureImporter {
public:
    U32 Load(const std::string& path, Vec2I& size) const;
    void UnLoad(U32& texture) const;
};
}

#endif
