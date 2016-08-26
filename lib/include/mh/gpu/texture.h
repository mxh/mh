#ifndef TEXTURE_H
#define TEXTURE_H 

#include "mh/base/defs.h"
#include "mh/base/imports.h"

#include <vector>

#include "mh/ext/gl3w/gl3w.h"

#include "mh/ext/soil/SOIL.h"

namespace mh
{
    
class Texture
{   
public:
    Texture (std::string filename);
    ~Texture();

    void   setTextureFile (std::string filename);
    
    GLuint getTextureID   (void)                 const { return m_textureID; }

protected:

private:
    GLuint m_textureID;

};

} // namespace mh 

#endif /* TEXTURE_H */

