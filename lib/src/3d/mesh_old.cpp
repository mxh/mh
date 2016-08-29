#include <algorithm>
#include <map>
#include <vector>

#include "eigen3/Eigen/Geometry"

#include "mh/3d/mesh.h"

#define MH_GEN_ARRAY_BUF(ID, TYPE, SIZE, START, GLTYPE, WIDTH, LOC) \
    glGenBuffers(1, &ID); \
    glBindBuffer(GL_ARRAY_BUFFER, ID); \
    glBufferData(GL_ARRAY_BUFFER, sizeof(TYPE) * SIZE, START, GL_STATIC_DRAW); \
    \
    glEnableVertexAttribArray(LOC);\
    glVertexAttribPointer(LOC, WIDTH, GLTYPE, 0, 0, 0)

#define MH_GEN_ELEM_ARRAY_BUF(ID, TYPE, SIZE, START) \
    glGenBuffers(1, &ID); \
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ID); \
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(TYPE) * SIZE, START, GL_STATIC_DRAW)

namespace mh
{

Mesh::Mesh(const std::vector<std::shared_ptr<Vertex> > & verts,
           const std::vector<std::shared_ptr<Face> > & faces)
: m_verts            (verts),
  m_faces            (faces),
  m_diffuse_texture  (nullptr),
  m_hasTextureCoords (false),
  m_position         (Eigen::Vector3f::Zero()),
  m_scale            (1.0f),
  m_rotation         (Eigen::AngleAxisf(0, Eigen::Vector3f::UnitY())),
  m_VBOCreated       (false),
  m_dirty            (true),
  m_minmax_dirty     (true)
{
    init();
}

Mesh::Mesh(const Mesh & other)
    : m_verts            (other.m_verts),
      m_halfedges        (other.m_halfedges)
      m_faces            (other.m_faces),
      m_diffuse_texture  (other.m_diffuse_texture),
      m_hasTextureCoords (other.m_hasTextureCoords),
      m_position         (other.m_position),
      m_scale            (other.m_scale),
      m_rotation         (Eigen::AngleAxisf(0, Eigen::Vector3f::UnitY())),
      m_min              (other.m_min),
      m_max              (other.m_max),
      m_VBOCreated       (false),
      m_dirty            (true),
      m_minmax_dirty     (true)
{
    init();
}

void swap(Mesh & first, Mesh & second)
{
    using std::swap;

    swap(first.m_verts,             second.m_verts);
    swap(first.m_halfedges,         second.m_halfedges);
    swap(first.m_faces,             second.m_faces);

    swap(first.m_diffuse_texture,   second.m_diffuse_texture);
    swap(first.m_hasTextureCoords,  second.m_hasTextureCoords);

    swap(first.m_position,          second.m_position);
    swap(first.m_scale,             second.m_scale);
    swap(first.m_rotation,          second.m_rotation);

    swap(first.m_min,               second.m_min);
    swap(first.m_max,               second.m_max);

    swap(first.m_bvh,               second.m_bvh);

    swap(first.m_dirty,             second.m_dirty);
    swap(first.m_minmax_dirty,      second.m_minmax_dirty);

    swap(first.m_vaoID,             second.m_vaoID);
    swap(first.m_indexVboID,        second.m_indexVboID);
    swap(first.m_posVboID,          second.m_posVboID);
    swap(first.m_normalVboID,       second.m_normalVboID);
    swap(first.m_colorVboID,        second.m_colorVboID);
}

Mesh & Mesh::operator=(Mesh other)
{
    swap(*this, other);

    return *this;
}

void Mesh::init()
{
    // set GL vars
    m_vaoID        = 0;
    m_indexVboID   = 0;
    m_posVboID     = 0;
    m_normalVboID  = 0;
    m_colorVboID   = 0;
    m_textureVboID = 0;

    // construct half-edge structure

    // create HalfEdges
    m_halfedges.reserve(3 * m_faces.size());
    for (size_t i = 0; i < m_faces.size(); ++i)
    {
        auto he_1 = std::make_shared<HalfEdge>();
        auto he_2 = std::make_shared<HalfEdge>();
        auto he_3 = std::make_shared<HalfEdge>();

        he_1->setVertex(m_verts[m_faces[i][0]]);
        he_2->setVertex(m_verts[m_faces[i][1]]);
        he_3->setVertex(m_verts[m_faces[i][2]]);

        he_1->setFace(face);
        he_2->setFace(face);
        he_3->setFace(face);

        he_1->setNext(he_2);
        he_2->setNext(he_3);
        he_3->setNext(he_1);

        m_faces[i]->getFaceVerts()[2]->getParent()->setHalfEdge(he_1);
        m_faces[i]->getFaceVerts()[0]->getParent()->setHalfEdge(he_2);
        m_faces[i]->getFaceVerts()[1]->getParent()->setHalfEdge(he_3);

        face->setHalfEdge(he_1);

        m_halfedges.push_back(he_1);
        m_halfedges.push_back(he_2);
        m_halfedges.push_back(he_3);

        m_verts[m_faceData[i][0]]->addFace(face);
        m_verts[m_faceData[i][1]]->addFace(face);
        m_verts[m_faceData[i][2]]->addFace(face);
    }

    // find twin edges
    std::map<std::pair<int, int>, std::shared_ptr<HalfEdge> > pairsVisited;
    std::vector<std::pair<int, int> > pairsDone;
    for (size_t i = 0; i < m_halfedges.size(); ++i)
    {
        int idxA = m_halfedges[i]->getVertex()->idx();
        int idxB = m_halfedges[i]->getNext()->getNext()->getVertex()->idx();
        if (idxA > idxB)
        {
            int tmp = idxA;
            idxA    = idxB;
            idxB    = tmp;
        }
        auto mapLoc = pairsVisited.find(std::pair<int, int>(idxA, idxB));
        if (mapLoc != pairsVisited.end())
        {
            m_halfedges[i]->setTwin(mapLoc->second);
            mapLoc->second->setTwin(m_halfedges[i]);
            pairsDone.push_back(std::pair<int, int>(idxA, idxB));
        } else {
            pairsVisited.insert(std::pair<std::pair<int, int>, std::shared_ptr<HalfEdge> >(
                std::pair<int, int>(idxA, idxB), m_halfedges[i]
            ));
        }
    }
}

std::vector<Eigen::Vector3i> Mesh::getFVI() const
{
    std::vector<Eigen::Vector3i> fvi;
    for (size_t i = 0; i < nFaces(); ++i)
    {
        int idxA = m_faces[i]->getHalfEdge()->getVertex()->idx();
        int idxB = m_faces[i]->getHalfEdge()->getNext()->getVertex()->idx();
        int idxC = m_faces[i]->getHalfEdge()->getNext()->getNext()->getVertex()->idx();

        fvi.push_back(Eigen::Vector3i(idxA, idxB, idxC));
    }

    return fvi;
}

void Mesh::draw(void) const
{
    update();

    glBindVertexArray(m_vaoID);
    glDrawElements(GL_TRIANGLES, 3 * nFaces(), GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}

void Mesh::createVBO(void) const
{
    glGenVertexArrays(1, &m_vaoID);
    glBindVertexArray(m_vaoID);

    // array buffers
    MH_GEN_ARRAY_BUF(m_posVboID,    Eigen::Vector3f, nVerts(), &m_vertData[0](0),   GL_FLOAT, 3, POSITION_LOCATION);
    MH_GEN_ARRAY_BUF(m_normalVboID, Eigen::Vector3f, nVerts(), &m_normalData[0](0), GL_FLOAT, 3, NORMAL_LOCATION);
    MH_GEN_ARRAY_BUF(m_colorVboID,  Eigen::Vector4f, nVerts(), &m_colorData[0](0),  GL_FLOAT, 4, COLOR_LOCATION);

    if (m_diffuse_texture && m_hasTextureCoords)
    {
        std::cout << "Uploading texture coords" << std::endl;
        MH_GEN_ARRAY_BUF(m_textureVboID, float2, nVerts(), &m_textureCoordsData[0][0], GL_FLOAT, 2, TEXTURE_LOCATION);
    } 
    updateMinMax();

    // element array buffers
    std::vector<Eigen::Vector3i> fvi = getFVI();
    MH_GEN_ELEM_ARRAY_BUF(m_indexVboID, Eigen::Vector3i, nFaces(), &fvi[0](0));

    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

}

void Mesh::deleteVBO(void) const
{
    glDeleteVertexArrays(1, &m_vaoID);
    glDeleteBuffers     (1, &m_indexVboID);
    glDeleteBuffers     (1, &m_posVboID);
    glDeleteBuffers     (1, &m_normalVboID);
    if (m_diffuse_texture && m_hasTextureCoords)
    {
        std::cout << "Deleting texture coords" << std::endl;
        glDeleteBuffers     (1, &m_textureVboID);
    }
    glDeleteBuffers     (1, &m_colorVboID);
}

void Mesh::update(bool force) const
{
    if (m_dirty || force)
    {
        if (m_VBOCreated)
        {
            deleteVBO();
        }
        createVBO();
        m_bvh = constructBVHFromMesh(this);

        m_dirty = false;
    }
}

void Mesh::updateMinMax() const
{
    if (m_minmax_dirty)
    {
        m_min = Eigen::Map<Eigen::Matrix<float, -1, 3, Eigen::RowMajor> >(const_cast<float*>(&m_vertData[0](0)), nVerts(), 3).colwise().minCoeff();
        m_max = Eigen::Map<Eigen::Matrix<float, -1, 3, Eigen::RowMajor> >(const_cast<float*>(&m_vertData[0](0)), nVerts(), 3).colwise().maxCoeff();
    }

    m_minmax_dirty = false;
}

Eigen::Vector3f Mesh::getCenter(void) const
{
    updateMinMax();
    return getModelToWorld() * ((m_min + m_max) / 2.0f);
}

Eigen::Affine3f Mesh::getModelToWorld(void) const
{
    return static_cast<Eigen::Affine3f>(Eigen::Translation3f(m_position)) * Eigen::UniformScaling<float>(m_scale) * m_rotation;
}

std::shared_ptr<BVH> constructBVHFromMesh(const Mesh * mesh)
{
    std::vector<std::shared_ptr<BVH> > faceBVHs;

    for (size_t i = 0; i < mesh->getFaces().size(); ++i)
    {
        faceBVHs.push_back(constructBVHFromFace(mesh->getFaces()[i]));
    }

    auto bvh = constructBVHFromSet(faceBVHs, BVH::X);

    return bvh;
}

std::shared_ptr<BVH> constructBVHFromMeshes(std::vector<std::shared_ptr<Mesh> > & meshes)
{
    std::vector<std::shared_ptr<BVH> > faceBVHs;

    for (size_t i = 0; i < meshes.size(); ++i)
    {
        for (size_t j = 0; j < meshes[i]->getFaces().size(); ++j)
        {
            faceBVHs.push_back(constructBVHFromFace(meshes[i]->getFaces()[j]));
        }
    }

    auto bvh = constructBVHFromSet(faceBVHs, BVH::X);

    return bvh;
}


} // namespace mh
