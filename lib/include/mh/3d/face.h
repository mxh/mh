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

    EXPOSE_SHARED_PTR(HalfEdge, HalfEdge, m_halfedge)

    int                           idx            (void)       const { return m_idx; }

    std::shared_ptr<const Vertex> getVertex      (size_t idx) const;
    std::shared_ptr<Vertex>       getVertex      (size_t idx);

    std::shared_ptr<Wedge>        getWedge       (size_t idx);
    std::shared_ptr<const Wedge>  getWedge       (size_t idx) const;

    Eigen::Vector3f               getEdge        (size_t idx) const;
    Eigen::Vector3f               getFaceNormal  (void)       const;

protected:

private:
    int                                        m_idx; // index in mesh

    std::shared_ptr<HalfEdge>                  m_halfedge;
    std::vector<std::shared_ptr<Wedge> >       m_wedges;

}; // class Face

} // namespace mh

#endif /* FACE_H */
