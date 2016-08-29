#ifndef MESH_H
#define MESH_H

#include <vector>

#include "mh/base/defs.h"
#include "mh/base/imports.h"

#include "mh/3d/vertex.h"
#include "mh/3d/face.h"
#include "mh/3d/halfedge.h"
#include "mh/3d/wedge.h"
#include "mh/3d/material.h"
#include "mh/3d/transform.h"

namespace mh
{

class Mesh
{
public:
    Mesh() {}

    const std::vector<std::shared_ptr<Vertex> >   & getVerts()     const { return m_verts; }
          std::vector<std::shared_ptr<Vertex> >   & getVerts()           { dirty(); return m_verts; }

    const std::vector<std::shared_ptr<Face> >     & getFaces()     const { return m_faces; }
          std::vector<std::shared_ptr<Face> >     & getFaces()           { dirty(); return m_faces; }

    const std::vector<std::shared_ptr<HalfEdge> > & getHalfEdges() const { return m_halfedges; }
          std::vector<std::shared_ptr<HalfEdge> > & getHalfEdges()       { dirty(); return m_halfedges; }

    const std::vector<std::shared_ptr<Wedge> >    & getWedges()    const { return m_wedges; }
          std::vector<std::shared_ptr<Wedge> >    & getWedges()          { dirty(); return m_wedges; }

    const std::shared_ptr<Material>               & getMaterial()  const { return m_material; }
          std::shared_ptr<Material>               & getMaterial()        { dirtyGL(); return m_material; }

    const Transform                               & getTransform() const { return m_transform; }
          Transform                               & getTransform()       { return m_transform; }
        
          size_t                                    nVerts()       const { return m_verts.size(); }
          size_t                                    nFaces()       const { return m_faces.size(); }

          Eigen::Vector3f                           getMin()       const { updateMinMax(); return m_min; }
          Eigen::Vector3f                           getMax()       const { updateMinMax(); return m_max; }
          Eigen::Vector3f                           getCenter()    const { updateMinMax(); return (m_min + m_max) / 2.0f; }
          
          size_t                                    idx()          const { return m_idx; }

          void                                      draw()         const { updateGL(); m_gl_state.draw(); }
    
protected:
    void dirty()   const { dirtyGL(); dirtyBB(); }
    void dirtyGL() const { m_dirty_gl = true; }
    void dirtyBB() const { m_dirty_bb = true; }

    void updateMinMax() const;
    void updateGL() const;

private:
    size_t                                  m_idx;

    std::vector<std::shared_ptr<Vertex> >   m_verts;
    std::vector<std::shared_ptr<Face> >     m_faces;
    std::vector<std::shared_ptr<HalfEdge> > m_halfedges;
    std::vector<std::shared_ptr<Wedge> >    m_wedges;

    std::shared_ptr<Material>               m_material;

    Transform                               m_transform;

    // cached variables & gl state
    mutable bool                            m_dirtyBB;
    mutable Eigen::Vector3f                 m_min;
    mutable Eigen::Vector3f                 m_max;

    mutable bool                            m_dirtyGL;
    mutable MeshGLState                     m_gl_state;

}; // class Mesh

} // namespace mh

#endif
