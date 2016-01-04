#ifndef FACE_H
#define FACE_H 

#include "mh/base/defs.h"
#include "mh/base/imports.h"

#include "mh/3d/halfedge.h"
#include "mh/3d/intersection.h"
#include "mh/3d/ray.h"
#include "mh/3d/vertex.h"

namespace mh
{

class Vertex;

class Face
{
public:
    Face(int idx) : m_idx(idx) {}

    EXPOSE_SHARED_PTR(HalfEdge, HalfEdge, m_halfedge)

    int                           idx             (void)       const { return m_idx; }

    std::shared_ptr<const Vertex> getVertex       (size_t idx) const;
    std::shared_ptr<Vertex>       getVertex       (size_t idx);
    Eigen::Vector3f               getEdge         (size_t idx) const;
    Eigen::Vector3f               getFaceNormal   (void)       const;

    Intersection<Face>            intersect       (const Ray & ray, bool backfacing=false);
    Eigen::Vector3f               getClosestPoint (Eigen::Vector3f point);

protected:

private:
    int                       m_idx; // index in mesh

    std::shared_ptr<HalfEdge> m_halfedge;

}; // class Face

} // namespace mh

#endif /* FACE_H */
