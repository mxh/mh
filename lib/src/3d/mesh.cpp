#include "mh/3d/mesh.h"

#include <map>
#include <memory>

#define COPY_VERTEX(IDX) \
    std::shared_ptr<Vertex> vert_##IDX; \
    auto vert_##IDX##_it = vert_idx_map.find(vert_##IDX##_idx); \
    if (vert_##IDX##_it != vert_idx_map.end()) \
    { \
        vert_##IDX = m_verts[vert_##IDX##_it->second]; \
    } else { \
        vert_##IDX = std::make_shared<Vertex>(mesh.getVerts()[vert_##IDX##_idx]->getPosition(), mesh.getVerts()[vert_##IDX##_idx]->idx()); \
        m_verts.push_back(vert_##IDX); \
        vert_idx_map[vert_##IDX##_idx] = m_verts.size() - 1; \
    }

#define COPY_VERTEX_SUBMESH(IDX) \
    std::shared_ptr<Vertex> vert_##IDX; \
    auto vert_##IDX##_it = vert_idx_map.find(vert_##IDX##_idx); \
    if (vert_##IDX##_it != vert_idx_map.end()) \
    { \
        vert_##IDX = submesh->getVerts()[vert_##IDX##_it->second]; \
    } else { \
        vert_##IDX = std::make_shared<Vertex>(mesh.getVerts()[vert_##IDX##_idx]->getPosition(), submesh->getVerts().size()); \
        submesh->getVerts().push_back(vert_##IDX); \
        vert_idx_map[vert_##IDX##_idx] = submesh->getVerts().size() - 1; \
    }

namespace mh
{

Mesh::Mesh(const Mesh & mesh)
    : m_transform(mesh.m_transform),
      m_dirtyBB(true),
      m_dirtyGL(true),
      m_gl_state(*this),
      m_pointcloud_gl_state(*this)
{
    std::map<size_t, size_t> vert_idx_map;
    for (size_t i = 0; i < mesh.m_faces.size(); ++i)
    {
        auto oldFace = mesh.getFaces()[i];
        auto face = std::make_shared<Face>(i);
        m_faces.push_back(face);

        size_t vert_0_idx = oldFace->getVertex(0)->idx();
        size_t vert_1_idx = oldFace->getVertex(1)->idx();
        size_t vert_2_idx = oldFace->getVertex(2)->idx();

        COPY_VERTEX(0);
        COPY_VERTEX(1);
        COPY_VERTEX(2);

        auto he_0 = std::make_shared<HalfEdge>();
        auto he_1 = std::make_shared<HalfEdge>();
        auto he_2 = std::make_shared<HalfEdge>();
        m_halfedges.push_back(he_0);
        m_halfedges.push_back(he_1);
        m_halfedges.push_back(he_2);

        he_0->setVertex(vert_0.get());
        he_1->setVertex(vert_1.get());
        he_2->setVertex(vert_2.get());

        he_0->setFace(face.get());
        he_1->setFace(face.get());
        he_2->setFace(face.get());

        he_0->setNext(he_1.get());
        he_1->setNext(he_2.get());
        he_2->setNext(he_0.get());

        vert_2->setHalfEdge(he_0.get());
        vert_0->setHalfEdge(he_1.get());
        vert_1->setHalfEdge(he_2.get());

        face->setHalfEdge(he_0.get());

        face->getWedges().resize(3);

        auto wedge_0 = std::make_shared<Wedge>(vert_0.get(), face.get());
        auto wedge_1 = std::make_shared<Wedge>(vert_1.get(), face.get());
        auto wedge_2 = std::make_shared<Wedge>(vert_2.get(), face.get());
        face->getWedges()[0] = wedge_0.get();
        face->getWedges()[1] = wedge_1.get();
        face->getWedges()[2] = wedge_2.get();
        m_wedges.push_back(wedge_0);
        m_wedges.push_back(wedge_1);
        m_wedges.push_back(wedge_2);

        face->getWedges()[0]->setTextureCoords(oldFace->getWedges()[0]->getTextureCoords());
        face->getWedges()[1]->setTextureCoords(oldFace->getWedges()[1]->getTextureCoords());
        face->getWedges()[2]->setTextureCoords(oldFace->getWedges()[2]->getTextureCoords());

        face->getWedges()[0]->setNormal(oldFace->getWedges()[0]->getNormal());
        face->getWedges()[1]->setNormal(oldFace->getWedges()[1]->getNormal());
        face->getWedges()[2]->setNormal(oldFace->getWedges()[2]->getNormal());
    }
    this->setMaterial(std::make_shared<Material>(*mesh.getMaterial()));
}

void Mesh::updateMinMax() const
{
    if (!m_dirtyBB) return;

    m_min = m_verts[0]->getPosition();
    m_max = m_verts[0]->getPosition();

    for (size_t i = 1; i < nVerts(); ++i)
    {
        m_min(0) = std::min(m_verts[i]->getPosition()(0), m_min(0));
        m_min(1) = std::min(m_verts[i]->getPosition()(1), m_min(1));
        m_min(2) = std::min(m_verts[i]->getPosition()(2), m_min(2));

        m_max(0) = std::max(m_verts[i]->getPosition()(0), m_max(0));
        m_max(1) = std::max(m_verts[i]->getPosition()(1), m_max(1));
        m_max(2) = std::max(m_verts[i]->getPosition()(2), m_max(2));
    }

    m_dirtyBB = false;
}

void Mesh::updateGL() const
{
    if (!m_dirtyGL) return;

    m_gl_state.deleteVBO();
    m_gl_state.createVBO();
    m_pointcloud_gl_state.deleteVBO();
    m_pointcloud_gl_state.createVBO();

    m_dirtyGL = false;
}

std::vector<Eigen::Vector3i> meshToFVI(const Mesh & mesh)
{
    std::vector<Eigen::Vector3i> fvi(mesh.nFaces());
    for (size_t i = 0; i < mesh.nFaces(); ++i)
    {
        fvi[i] = {mesh.getFaces()[i]->getVertex(0)->idx(),
                  mesh.getFaces()[i]->getVertex(1)->idx(),
                  mesh.getFaces()[i]->getVertex(2)->idx()};
    }

    return fvi;
}

std::unique_ptr<Mesh> getSubMesh(const Mesh & mesh, const std::vector<size_t> & faceIndices)
{
    auto submesh = std::make_unique<Mesh>();

    std::map<size_t, size_t> vert_idx_map;
    for (size_t i = 0; i < faceIndices.size(); ++i)
    {
        auto oldFace = mesh.getFaces()[faceIndices[i]];
        auto face = std::make_shared<Face>(i);
        submesh->getFaces().push_back(face);

        size_t vert_0_idx = oldFace->getVertex(0)->idx();
        size_t vert_1_idx = oldFace->getVertex(1)->idx();
        size_t vert_2_idx = oldFace->getVertex(2)->idx();

        COPY_VERTEX_SUBMESH(0);
        COPY_VERTEX_SUBMESH(1);
        COPY_VERTEX_SUBMESH(2);

        auto he_0 = std::make_shared<HalfEdge>();
        auto he_1 = std::make_shared<HalfEdge>();
        auto he_2 = std::make_shared<HalfEdge>();
        submesh->getHalfEdges().push_back(he_0);
        submesh->getHalfEdges().push_back(he_1);
        submesh->getHalfEdges().push_back(he_2);

        he_0->setVertex(vert_0.get());
        he_1->setVertex(vert_1.get());
        he_2->setVertex(vert_2.get());

        he_0->setFace(face.get());
        he_1->setFace(face.get());
        he_2->setFace(face.get());

        he_0->setNext(he_1.get());
        he_1->setNext(he_2.get());
        he_2->setNext(he_0.get());

        vert_2->setHalfEdge(he_0.get());
        vert_0->setHalfEdge(he_1.get());
        vert_1->setHalfEdge(he_2.get());

        face->setHalfEdge(he_0.get());

        face->getWedges().resize(3);

        auto wedge_0 = std::make_shared<Wedge>(vert_0.get(), face.get());
        auto wedge_1 = std::make_shared<Wedge>(vert_1.get(), face.get());
        auto wedge_2 = std::make_shared<Wedge>(vert_2.get(), face.get());
        face->getWedges()[0] = wedge_0.get();
        face->getWedges()[1] = wedge_1.get();
        face->getWedges()[2] = wedge_2.get();
        submesh->getWedges().push_back(wedge_0);
        submesh->getWedges().push_back(wedge_1);
        submesh->getWedges().push_back(wedge_2);

        face->getWedges()[0]->setTextureCoords(oldFace->getWedges()[0]->getTextureCoords());
        face->getWedges()[1]->setTextureCoords(oldFace->getWedges()[1]->getTextureCoords());
        face->getWedges()[2]->setTextureCoords(oldFace->getWedges()[2]->getTextureCoords());

        face->getWedges()[0]->setNormal(oldFace->getWedges()[0]->getNormal());
        face->getWedges()[1]->setNormal(oldFace->getWedges()[1]->getNormal());
        face->getWedges()[2]->setNormal(oldFace->getWedges()[2]->getNormal());
    }
    submesh->setMaterial(std::make_shared<Material>(*mesh.getMaterial()));

    return submesh;
}

void recomputeNormalsMesh(Mesh & mesh)
{
    for (size_t i = 0; i < mesh.getFaces().size(); ++i)
    {
        auto vertex_0 = mesh.getFaces()[i]->getVertex(0);
        auto vertex_1 = mesh.getFaces()[i]->getVertex(1);
        auto vertex_2 = mesh.getFaces()[i]->getVertex(2);

        Eigen::Vector3f normal = (vertex_1->getPosition() - vertex_0->getPosition()).cross(
                                 (vertex_2->getPosition() - vertex_0->getPosition()));

        mesh.getFaces()[i]->getWedges()[0]->setNormal(normal);
        mesh.getFaces()[i]->getWedges()[1]->setNormal(normal);
        mesh.getFaces()[i]->getWedges()[2]->setNormal(normal);
    }
}

} // namespace mh
