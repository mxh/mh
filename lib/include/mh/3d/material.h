#include <memory>

#include "mh/base/defs.h"
#include "mh/base/imports.h"

#include "mh/gpu/texture.h"

namespace mh
{

class Material
{
public:
    Material() : m_diffuse(0.0f, 0.0f, 0.0f),
                 m_specular(0.0f, 0.0f, 0.0f),
                 m_shininess(0.0f),
                 m_diffuse_texture(nullptr),
                 m_specular_texture(nullptr) {}

    void            setDiffuse(Eigen::Vector3f diffuse)         { m_diffuse = std::move(diffuse); }
    Eigen::Vector3f getDiffuse()                          const { return m_diffuse; }

    void            setSpecular(Eigen::Vector3f specular)       { m_specular = std::move(specular); }
    Eigen::Vector3f getSpecular()                         const { return m_specular; }

    void            setShininess(float shininess)               { m_shininess = shininess; }
    float           getShininess()                        const { return m_shininess; }

    bool            hasTexture()                          const { return m_diffuse_texture != nullptr; }

protected:

private:
    Eigen::Vector3f          m_diffuse;
    Eigen::Vector3f          m_specular;
    float                    m_shininess;

    std::shared_ptr<Texture> m_diffuse_texture;
    std::shared_ptr<Texture> m_specular_texture;

}; // class Material

} // namespace mh
