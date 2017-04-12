#ifndef HALFEDGE_H
#define HALFEDGE_H 

#include "mh/base/defs.h"
#include "mh/base/imports.h"

namespace mh
{

class Face;
class Vertex;

class HalfEdge
{
public:
    HalfEdge() {}

    EXPOSE_PTR(Vertex,   Vertex, m_vertex);
    EXPOSE_PTR(Face,     Face,   m_face);
    EXPOSE_PTR(HalfEdge, Next,   m_next);
    EXPOSE_PTR(HalfEdge, Twin,   m_twin);

protected:

private:
    Vertex *   m_vertex;
    Face *     m_face;
    HalfEdge * m_next;
    HalfEdge * m_twin;

}; // class HalfEdge

} // namespace mh

#endif /* HALFEDGE_H */
