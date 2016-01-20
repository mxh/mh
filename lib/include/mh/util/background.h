#ifndef BACKGROUND_H
#define BACKGROUND_H 

#include <memory>

#include "mh/gpu/shader.h"
#include "mh/gpu/texture.h"

namespace mh
{

class Background
{
public:
    Background(const std::string & filename);

    void draw();

protected:

private:
    std::shared_ptr<Shader>  m_shader;
    std::shared_ptr<Texture> m_texture;

}; // class Background

} // namespace mh

#endif /* BACKGROUND_H */
