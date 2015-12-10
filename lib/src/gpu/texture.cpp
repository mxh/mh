#include "mh/gpu/texture.h"

#include <iostream>

namespace mh
{

Texture::Texture(std::string filename)
{
    m_textureID = SOIL_load_OGL_texture(
        filename.c_str(),
        SOIL_LOAD_AUTO,
        SOIL_CREATE_NEW_ID,
        SOIL_FLAG_MIPMAPS | SOIL_FLAG_INVERT_Y | SOIL_FLAG_NTSC_SAFE_RGB | SOIL_FLAG_COMPRESS_TO_DXT
    );

    if (m_textureID == 0)
    {
        std::cerr << "Warning: texture loading failed: " << SOIL_last_result() << std::endl;
    }

    MH_ASSERT(m_textureID != 0);
}

void Texture::setTextureFile(std::string filename)
{
    m_textureID = SOIL_load_OGL_texture(
        filename.c_str(),
        SOIL_LOAD_AUTO,
        SOIL_CREATE_NEW_ID,
        SOIL_FLAG_MIPMAPS | SOIL_FLAG_INVERT_Y | SOIL_FLAG_NTSC_SAFE_RGB | SOIL_FLAG_COMPRESS_TO_DXT
    );

    if (m_textureID == 0)
    {
        std::cerr << "Warning: texture loading failed: " << SOIL_last_result() << std::endl;
    }

    MH_ASSERT(m_textureID != 0);
}

} // namespace mh 
