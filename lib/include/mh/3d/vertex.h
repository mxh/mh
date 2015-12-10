#ifndef VERTEX_H
#define VERTEX_H 

#include "mh/base/defs.h"
#include "mh/base/imports.h"

namespace mh
{

class HalfEdge;

class Vertex
{
public:
    Vertex(Eigen::Vector3f & position,
           Eigen::Vector3f & normal,
           Eigen::Vector4f & color,
           Eigen::Vector2f & textureCoords,
           int               idx)
    : m_position(position),
      m_normal(normal),
      m_color(color),
      m_textureCoords(textureCoords),
      m_idx(idx) {}

    EXPOSE            (Eigen::Vector3f, Position,      m_position)
    EXPOSE            (Eigen::Vector3f, Normal,        m_normal)
    EXPOSE            (Eigen::Vector4f, Color,         m_color)
    EXPOSE            (Eigen::Vector2f, TextureCoords, m_textureCoords);

    EXPOSE_SHARED_PTR (HalfEdge,        HalfEdge, m_halfedge)

    int idx (void) const { return m_idx; }

protected:

private:
    Eigen::Vector3f &         m_position;
    Eigen::Vector3f &         m_normal;
    Eigen::Vector4f &         m_color;
    Eigen::Vector2f &         m_textureCoords;

    int                       m_idx; // index in mesh

    std::shared_ptr<HalfEdge> m_halfedge;

}; // class Vertex

} // namespace mh

#endif /* VERTEX_H */
