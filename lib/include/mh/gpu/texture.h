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

    void                setTextureFile(std::string filename);
    const std::string & getTextureFile() const { return m_filename; }
    
    GLuint getTextureID   (void)         const;

protected:
    void load() const;

private:
    mutable bool   m_isLoaded;
    mutable GLuint m_textureID;
    std::string    m_filename;

};

} // namespace mh 

#endif /* TEXTURE_H */

