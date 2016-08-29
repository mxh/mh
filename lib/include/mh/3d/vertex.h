#ifndef VERTEX_H
#define VERTEX_H 

#include "mh/base/defs.h"
#include "mh/base/imports.h"

namespace mh
{

class HalfEdge;
class Face;

class Vertex
{
public:
    Vertex(Eigen::Vector3f position,
           Eigen::Vector3f normal,
           int idx)
    : m_position(position),
      m_normal(normal),
      m_idx(idx) {}

    EXPOSE            (Eigen::Vector3f, Position,      m_position)
    EXPOSE            (Eigen::Vector3f, Normal,        m_normal)

    EXPOSE_SHARED_PTR (HalfEdge,        HalfEdge, m_halfedge)

    void addFace(std::shared_ptr<Face> face) { m_faces.push_back(face); }
    std::vector<std::shared_ptr<Face> > getFaces() { return m_faces; }

    int idx (void) const { return m_idx; }

protected:

private:
    Eigen::Vector3f                     m_position;
    Eigen::Vector3f                     m_normal;

    int                                 m_idx; // index in mesh

    std::shared_ptr<HalfEdge>           m_halfedge;
    std::vector<std::shared_ptr<Face> > m_faces;

}; // class Vertex

} // namespace mh

#endif /* VERTEX_H */
