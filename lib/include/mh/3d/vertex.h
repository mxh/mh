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
    : m_position(position),
      m_color(0.0f, 0.0f, 0.0f),
      m_idx(idx) {}

    EXPOSE            (Eigen::Vector3f, Position,      m_position)
    EXPOSE            (Eigen::Vector3f, Color,         m_color)

    EXPOSE_SHARED_PTR (HalfEdge,        HalfEdge, m_halfedge)

    void addWedge(std::shared_ptr<Wedge> wedge)                    { m_wedges.push_back(wedge); }
    const std::vector<std::shared_ptr<Wedge> > & getWedges() const { return m_wedges; }
          std::vector<std::shared_ptr<Wedge> > & getWedges()       { return m_wedges; }

    int idx (void) const { return m_idx; }

protected:

private:
    Eigen::Vector3f                      m_position;
    Eigen::Vector3f                      m_color;

    int                                  m_idx; // index in mesh

    std::shared_ptr<HalfEdge>            m_halfedge;
    std::vector<std::shared_ptr<Wedge> > m_wedges;

}; // class Vertex

} // namespace mh

#endif /* VERTEX_H */
