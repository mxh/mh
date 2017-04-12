#ifndef FACE_H
#define FACE_H 

#include "mh/base/defs.h"
#include "mh/base/imports.h"

#include "mh/3d/halfedge.h"
#include "mh/3d/vertex.h"
#include "mh/3d/wedge.h"

namespace mh
{

class Vertex;

class Face
{
public:
    Face(int idx) : m_idx(idx) {}

    EXPOSE_PTR(HalfEdge, HalfEdge, m_halfedge)

    int                                          idx()                 const { return m_idx; }

    const Vertex * getVertex(size_t idx) const;
          Vertex * getVertex(size_t idx);

    const std::vector<Wedge *> & getWedges() const { return m_wedges; }
          std::vector<Wedge *> & getWedges()       { return m_wedges; }

    Eigen::Vector3f               getEdge        (size_t idx)          const;
    Eigen::Vector3f               getFaceNormal  (void)                const;

protected:

private:
    int                  m_idx; // index in mesh

    HalfEdge *           m_halfedge;
    std::vector<Wedge *> m_wedges;

}; // class Face

} // namespace mh

#endif /* FACE_H */
