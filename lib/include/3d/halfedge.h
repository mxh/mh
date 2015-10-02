#ifndef HALFEDGE_H
#define HALFEDGE_H 

#include "base/defs.h"
#include "base/imports.h"

namespace lp
{

class Face;
class Vertex;

class HalfEdge
{
public:
    HalfEdge() {}

    EXPOSE_SHARED_PTR(Vertex,   Vertex, m_vertex);
    EXPOSE_SHARED_PTR(Face,     Face,   m_face);
    EXPOSE_SHARED_PTR(HalfEdge, Next,   m_next);
    EXPOSE_SHARED_PTR(HalfEdge, Twin,   m_twin);

protected:

private:
    std::shared_ptr<Vertex>   m_vertex;
    std::shared_ptr<Face>     m_face;
    std::shared_ptr<HalfEdge> m_next;
    std::shared_ptr<HalfEdge> m_twin;

}; // class HalfEdge

} // namespace lp

#endif /* HALFEDGE_H */
