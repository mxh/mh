#ifndef WEDGE_H
#define WEDGE_H

#include <memory>

#include "mh/base/defs.h"
#include "mh/base/imports.h"

namespace mh
{

class Wedge
{
public:
    Wedge(std::shared_ptr<Vertex> vertex, std::shared_ptr<Face> face) : m_vertex(vertex), m_face(face), m_hasTextureCoords(false) {}

    std::shared_ptr<Vertex>       getVertex()       { return m_vertex; }
    std::shared_ptr<const Vertex> getVertex() const { return m_vertex; }

    std::shared_ptr<Face>         getFace()         { return m_face; }
    std::shared_ptr<const Face>   getFace()   const { return m_face; }

    void   setTextureCoords(float2 textureCoords)       { m_textureCoords = textureCoords; m_hasTextureCoords = true; }
    float2 getTextureCoords()                     const { return m_textureCoords; }
    bool   hasTextureCoords()                     const { return m_hasTextureCoords; }

    void   setNormal(Eigen::Vector3f normal)            { m_normal = normal;}
    Eigen::Vector3f getNormal()                   const { return m_normal; }

protected:

private:
    std::shared_ptr<Vertex> m_vertex;
    std::shared_ptr<Face>   m_face;

    bool                    m_hasTextureCoords;
    float2                  m_textureCoords;
    Eigen::Vector3f         m_normal;

}; // class Wedge

} // namespace mh

#endif
