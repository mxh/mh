#ifndef MESH_H
#define MESH_H

#include "mh/base/defs.h"
#include "mh/base/imports.h"

#include <vector>

//#include <GL/glew.h>
#include "mh/ext/gl3w/gl3w.h"

#include "eigen3/Eigen/Geometry"

#include "mh/3d/bvh.h"
#include "mh/3d/face.h"
#include "mh/3d/halfedge.h"
#include "mh/3d/vertex.h"

#include "mh/gpu/texture.h"

namespace mh
{

class Mesh
{
public:
    static constexpr int POSITION_LOCATION = 0;
    static constexpr int NORMAL_LOCATION   = 1;
    static constexpr int COLOR_LOCATION    = 2;
    static constexpr int TEXTURE_LOCATION  = 3;

public:
                                                    //Mesh      (void) : m_idx(-1), m_position(Eigen::Vector3f::Zero()), m_VBOCreated(false), m_dirty(false) {}
  
                                                    Mesh      (const std::vector<std::shared_ptr<Vertex> > & vertices,
                                                               const std::vector<std::shared_ptr<Face> > & faces)
                                                    Mesh      (const Mesh & other);

    friend void                                     swap      (Mesh & first, Mesh & second);
    Mesh &                                          operator= (      Mesh  other); // copy and swap

    const std::vector<std::shared_ptr<Vertex> > &   getVertices()    const {                    return m_verts; }
          std::vector<std::shared_ptr<Vertex> > &   getVertices()          { m_dirty = true; m_minmax_dirty = true; return m_verts; }

    const std::vector<std::shared_ptr<HalfEdge> > & getHalfEdges()   const {                    return m_halfedges; }
          std::vector<std::shared_ptr<HalfEdge> > & getHalfEdges()         { m_dirty = true; m_minmax_dirty = true; return m_halfedges; }

    const std::vector<std::shared_ptr<Face> > &     getFaces()       const {                    return m_faces; }
          std::vector<std::shared_ptr<Face> > &     getFaces()             { m_dirty = true; m_minmax_dirty = true; return m_faces; }

    bool                                            hasTextureCoords() const { return m_hasTextureCoords; }

    std::shared_ptr<BVH>                            getBVH()               { update();          return m_bvh; }

    Eigen::Vector3f                                 getDiffuse()     const { return m_diffuse; }
    void                                            setDiffuse(Eigen::Vector3f diffuse) { m_diffuse = diffuse; }

    Eigen::Vector3f                                 getSpecular()    const { return m_specular; }
    void                                            setSpecular(Eigen::Vector3f specular) { m_specular = specular; }

    float                                           getShininess()   const { return m_shininess; }
    void                                            setShininess(float shininess) { m_shininess = shininess; }

    const Texture &                                 getTexture()     const { return *m_diffuse_texture; }
    void                                            setTexture(std::shared_ptr<Texture> texture) { m_dirty = true; m_diffuse_texture = texture; }

    std::size_t                                     idx             (void) const { return m_idx; }

    std::vector<Eigen::Vector3i>                    getFVI          (void) const;
    void                                            draw            (void) const;
    void                                            createVBO       (void) const;
    void                                            deleteVBO       (void) const;
    void                                            update          (bool force=false) const;
    void                                            updateMinMax    (void) const;

    size_t                                          nVerts          (void) const { return m_verts.size(); }
    size_t                                          nFaces          (void) const { return m_faces.size(); }

    Eigen::Vector3f                                 getMin          (void) const { return m_min; }
    Eigen::Vector3f                                 getMax          (void) const { return m_max; }
    Eigen::Vector3f                                 getCenter       (void) const;

    Eigen::Vector3f                                 getPosition     (void) const { return m_position; }
    void                                            setPosition     (Eigen::Vector3f position) { m_position = position; }

    float                                           getScale        (void) const { return m_scale; }
    void                                            setScale        (float scale) { m_scale = scale; }

    Eigen::AngleAxisf                               getRotation     (void) const { return m_rotation; }
    void                                            setRotation     (Eigen::AngleAxisf rotation) { m_rotation = rotation; }

    Eigen::Affine3f                                 getModelToWorld (void) const;

protected:
    void                                            init            (void);

private:
    // index
    std::size_t                             m_idx;

    // structures
    std::vector<std::shared_ptr<Vertex> >   m_verts;
    std::vector<std::shared_ptr<HalfEdge> > m_halfedges;
    std::vector<std::shared_ptr<Face> >     m_faces;

    // material
    Eigen::Vector3f                         m_diffuse;
    Eigen::Vector3f                         m_specular;
    float                                   m_shininess;

    // texture
    std::shared_ptr<Texture>                m_diffuse_texture;
    bool                                    m_hasTextureCoords;

    // relative position
    Eigen::Vector3f                         m_position;

    // relative scale
    float                                   m_scale;

    // relative rotation
    Eigen::AngleAxisf                       m_rotation;

    // bounding box
    mutable Eigen::Vector3f                 m_min;
    mutable Eigen::Vector3f                 m_max;

    // bounding volume hierarchy
    mutable std::shared_ptr<BVH>                    m_bvh;

    // state
    mutable bool                                    m_VBOCreated;
    mutable bool                                    m_dirty;
    mutable bool                            m_minmax_dirty;

    // GL
    mutable GLuint                                  m_vaoID;
    mutable GLuint                                  m_indexVboID;
    mutable GLuint                                  m_posVboID;
    mutable GLuint                                  m_normalVboID;
    mutable GLuint                                  m_colorVboID;
    mutable GLuint                                  m_textureVboID;

}; // class Mesh

std::shared_ptr<BVH> constructBVHFromMesh(const Mesh * mesh);
std::shared_ptr<BVH> constructBVHFromMeshes(std::vector<std::shared_ptr<Mesh> > & meshes);

} // namespace mh

#endif /* MESH_H */
