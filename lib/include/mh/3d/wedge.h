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
    std::shared_ptr<Vertex>       getVertex()       { return m_vertex; }
    std::shared_ptr<const Vertex> getVertex() const { return m_vertex; }

    void   setTextureCoords(float2 textureCoords)       { m_textureCoords = textureCoords; }
    float2 getTextureCoords()                     const { return m_textureCoords; }

    void   setNormal(Eigen::Vector3f normal)            { m_normal = normal;}
    Eigen::Vector3f getNormal()                   const { return m_normal; }

protected:

private:
    std::shared_ptr<Vertex> m_vertex;

    float2                  m_textureCoords;
    Eigen::Vector3f         m_normal;

}; // class Wedge

} // namespace mh

#endif
