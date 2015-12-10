#ifndef MESH_H
#define MESH_H

#include "mh/base/defs.h"
#include "mh/base/imports.h"

#include <vector>

#include <GL/glew.h>

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
                                                    Mesh      (const std::vector<Eigen::Vector3f> & vertData,
                                                               const std::vector<Eigen::Vector3f> & normalData,
                                                               const std::vector<Eigen::Vector3i> & faceData);
                                                    Mesh      (const std::vector<Eigen::Vector3f> & vertData,
                                                               const std::vector<Eigen::Vector3f> & normalData,
                                                               const std::vector<Eigen::Vector4f> & colorData,
                                                               const std::vector<Eigen::Vector3i> & faceData);
                                                    Mesh      (const std::vector<Eigen::Vector3f> & vertData,
                                                               const std::vector<Eigen::Vector3f> & normalData,
                                                               const std::vector<float2>          & textureCoordsData,
                                                               const std::vector<Eigen::Vector3i> & faceData);
                                                    Mesh      (const Mesh& other);

    friend void                                     swap      (Mesh & first, Mesh & second);
    Mesh &                                          operator= (      Mesh  other); // copy and swap

    const std::vector<std::shared_ptr<Vertex> > &   getVertices()          const {                    return m_verts; }
          std::vector<std::shared_ptr<Vertex> > &   getVertices()                { m_dirty = true;    return m_verts; }

    const std::vector<std::shared_ptr<HalfEdge> > & getHalfEdges()         const {                    return m_halfedges; }
          std::vector<std::shared_ptr<HalfEdge> > & getHalfEdges()               { m_dirty = true;    return m_halfedges; }

    const std::vector<std::shared_ptr<Face> > &     getFaces()             const {                    return m_faces; }
          std::vector<std::shared_ptr<Face> > &     getFaces()                   { m_dirty = true;    return m_faces; }

    const std::vector<Eigen::Vector3f> &            getVertData()          const {                    return m_vertData; }
    const std::vector<Eigen::Vector3f> &            getNormalData()        const {                    return m_normalData; }
    const std::vector<Eigen::Vector4f> &            getColorData()         const {                    return m_colorData; }
    const std::vector<float2> &                     getTextureCoordsData() const {                    return m_textureCoordsData; }

    std::shared_ptr<Texture>                        getTexture()                                 { return m_texture; }
    void                                            setTexture(std::shared_ptr<Texture> texture) { m_texture = texture; setHasTexture(true); m_dirty = true; }

    std::shared_ptr<BVH>                            getBVH()                     { update(); return m_bvh; }

    std::size_t                                     idx                 (void) const            { return m_idx; }

    bool                                            getHasTextureCoords (void) const            { return m_hasTextureCoords; }
    void                                            setHasTextureCoords (bool hasTextureCoords) { m_hasTextureCoords = hasTextureCoords; }

    bool                                            getHasTexture       (void) const            { return m_hasTexture; }
    void                                            setHasTexture       (bool hasTexture)       { m_hasTexture = hasTexture; }

    std::vector<Eigen::Vector3i>                    getFVI              (void) const;
    void                                            draw                (void);
    void                                            createVBO           (void);
    void                                            deleteVBO           (void);
    void                                            update              (bool force=false);

    size_t                                          nVerts              (void) const { return m_vertData.size(); }
    size_t                                          nFaces              (void) const { return m_faces.size(); }

    Eigen::Vector3f                                 getMin              (void) const { return m_min; }
    Eigen::Vector3f                                 getMax              (void) const { return m_max; }
    Eigen::Vector3f                                 getCenter           (void);

    Eigen::Vector3f                                 getPosition         (void) const;
    void                                            setPosition         (Eigen::Vector3f position) { m_position = position; }

    Eigen::Affine3f                                 getModelToWorld     (void) const;

protected:
    void                                            init                (void);

private:
    // index
    std::size_t                             m_idx;

    // raw data - not accessible directly through public interface
    std::vector<Eigen::Vector3f>            m_vertData;
    std::vector<Eigen::Vector3f>            m_normalData;
    std::vector<Eigen::Vector4f>            m_colorData;
    std::vector<float2>                     m_textureCoordsData;
    std::vector<Eigen::Vector3i>            m_faceData;

    // structures
    std::vector<std::shared_ptr<Vertex> >   m_verts;
    std::vector<std::shared_ptr<HalfEdge> > m_halfedges;
    std::vector<std::shared_ptr<Face> >     m_faces;
    std::shared_ptr<Texture>                m_texture;

    // relative position
    Eigen::Vector3f                         m_position;

    // bounding box
    Eigen::Vector3f                         m_min;
    Eigen::Vector3f                         m_max;

    // bounding volume hierarchy
    std::shared_ptr<BVH>                    m_bvh;

    // state
    bool                                    m_VBOCreated;
    bool                                    m_hasTexture;
    bool                                    m_hasTextureCoords;
    bool                                    m_dirty;

    // GL
    GLuint                                  m_vaoID;
    GLuint                                  m_indexVboID;
    GLuint                                  m_posVboID;
    GLuint                                  m_normalVboID;
    GLuint                                  m_colorVboID;
    GLuint                                  m_textureVboID;

}; // class Mesh

std::shared_ptr<BVH> constructBVHFromMesh(Mesh * mesh);
std::shared_ptr<BVH> constructBVHFromMeshes(std::vector<std::shared_ptr<Mesh> > & meshes);

} // namespace mh

#endif /* MESH_H */
