#ifndef MATERIAL_H
#define MATERIAL_H

#include <memory>
#include <string>

#include "mh/base/defs.h"
#include "mh/base/imports.h"

#include "mh/gpu/texture.h"

namespace mh
{

class Material
{
public:
    Material() : m_diffuse(0.4f, 0.4f, 0.4f),
                 m_specular(0.4f, 0.4f, 0.4f),
                 m_shininess(1.0f),
                 m_diffuse_texture(nullptr),
                 m_specular_texture(nullptr) {}

    void                setName(std::string name)                   { m_name = std::move(name); }
    const std::string & getName()                             const { return m_name; }

    void                setDiffuse(Eigen::Vector3f diffuse)         { m_diffuse = std::move(diffuse); }
    Eigen::Vector3f     getDiffuse()                          const { return m_diffuse; }

    void                setSpecular(Eigen::Vector3f specular)       { m_specular = std::move(specular); }
    Eigen::Vector3f     getSpecular()                         const { return m_specular; }

    void                setShininess(float shininess)               { m_shininess = shininess; }
    float               getShininess()                        const { return m_shininess; }

    void                           setDiffuseTexture(std::shared_ptr<Texture> texture) { m_diffuse_texture = texture; }
    std::shared_ptr<const Texture> getDiffuseTexture() const                           { return m_diffuse_texture; }
    //std::shared_ptr<Texture>       getDiffuseTexture()                                 { return m_diffuse_texture; }

    void                           setSpecularTexture(std::shared_ptr<Texture> texture) { m_specular_texture = texture; }
    std::shared_ptr<const Texture> getSpecularTexture() const                           { return m_specular_texture; }
    //std::shared_ptr<Texture>       getSpecularTexture()                                 { return m_specular_texture; }

    bool            hasTexture()                          const { return m_diffuse_texture != nullptr; }

protected:

private:
    std::string              m_name;
    Eigen::Vector3f          m_diffuse;
    Eigen::Vector3f          m_specular;
    float                    m_shininess;

    std::shared_ptr<Texture> m_diffuse_texture;
    std::shared_ptr<Texture> m_specular_texture;

}; // class Material

} // namespace mh

#endif
