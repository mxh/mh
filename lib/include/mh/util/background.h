#ifndef BACKGROUND_H
#define BACKGROUND_H 

#include <memory>
#include <string>

#include "mh/gpu/shader.h"
#include "mh/gpu/texture.h"

namespace mh
{

class Background
{
public:
    Background(const std::string & filename);

    const std::string & get_filename() const { return m_filename; }

    void draw();

protected:

private:
    std::shared_ptr<Shader>  m_shader;
    std::shared_ptr<Texture> m_texture;

    std::string              m_filename;

    GLuint                   m_vao;
    GLuint                   m_vbo;

}; // class Background

} // namespace mh

#endif /* BACKGROUND_H */
