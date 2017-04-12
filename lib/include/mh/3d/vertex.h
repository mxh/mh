#ifndef VERTEX_H
#define VERTEX_H 

#include "mh/base/defs.h"
#include "mh/base/imports.h"

namespace mh
{

class HalfEdge;
class Wedge;

class Vertex
{
public:
    Vertex(Eigen::Vector3f position,
           int idx)
    : m_position(position)
    , m_color(0.0f, 0.0f, 0.0f)
    , m_idx(idx)
    , m_custom_int(0)
    , m_custom_vec(0.0f, 0.0f, 0.0f) {}

    EXPOSE     (Eigen::Vector3f, Position, m_position)
    EXPOSE     (Eigen::Vector3f, Color,    m_color)

    EXPOSE_PTR (HalfEdge,        HalfEdge, m_halfedge)

    void addWedge(Wedge * wedge)                   { m_wedges.push_back(wedge); }
    const std::vector<Wedge *> & getWedges() const { return m_wedges; }
          std::vector<Wedge *> & getWedges()       { return m_wedges; }

    int idx (void) const { return m_idx; }

    EXPOSE            (int,             CustomInt,     m_custom_int)
    EXPOSE            (Eigen::Vector3f, CustomVec,     m_custom_vec)

protected:

private:
    Eigen::Vector3f      m_position;
    Eigen::Vector3f      m_color;

    int                  m_idx; // index in mesh

    int                  m_custom_int;
    Eigen::Vector3f      m_custom_vec;

    HalfEdge *           m_halfedge;
    std::vector<Wedge *> m_wedges;

}; // class Vertex

} // namespace mh

#endif /* VERTEX_H */
