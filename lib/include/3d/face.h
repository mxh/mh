#ifndef FACE_H
#define FACE_H 

#include "base/defs.h"
#include "base/imports.h"

#include "3d/halfedge.h"
#include "3d/vertex.h"

namespace lp
{

class Vertex;

class Face
{
public:
    Face(int idx) : m_idx(idx) {}

    EXPOSE_SHARED_PTR(HalfEdge, HalfEdge, m_halfedge)

    int                           idx           (void)       const { return m_idx; }

    std::shared_ptr<const Vertex> getVertex     (size_t idx) const;
    std::shared_ptr<Vertex>       getVertex     (size_t idx);
    Eigen::Vector3f               getEdge       (size_t idx) const;
    Eigen::Vector3f               getFaceNormal (void)       const;

protected:

private:
    int                       m_idx; // index in mesh

    std::shared_ptr<HalfEdge> m_halfedge;

}; // class Face

} // namespace lp

#endif /* FACE_H */
