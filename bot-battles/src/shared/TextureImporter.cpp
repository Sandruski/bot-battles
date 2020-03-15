#include "TextureImporter.h"

namespace sand {

//----------------------------------------------------------------------------------------------------
U32 TextureImporter::Load(const std::string& path, Vec2I& size) const
{
    stbi_set_flip_vertically_on_load(true);

    int x, y, channels;
    U8* data = stbi_load(path.c_str(), &x, &y, &channels, STBI_rgb_alpha);
    if (data == nullptr) {
        ELOG("Texture could not be loaded");
        return 0;
    }

    U32 texture = 0;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, x, y, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D);

    stbi_image_free(data);

    size.x = x;
    size.y = y;

    return texture;
}

//----------------------------------------------------------------------------------------------------
void TextureImporter::UnLoad(U32& texture) const
{
    glDeleteTextures(1, &texture);
    texture = 0;
}
}
