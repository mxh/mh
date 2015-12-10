#include <map>
#include <vector>

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

Mesh::Mesh(const std::vector<Eigen::Vector3f> & vertData,
           const std::vector<Eigen::Vector3f> & normalData,
           const std::vector<Eigen::Vector3i> & faceData)
: m_vertData         (vertData),
  m_normalData       (normalData),
  m_faceData         (faceData),
  m_position         (Eigen::Vector3f::Zero()),
  m_VBOCreated       (false),
  m_hasTexture       (false),
  m_hasTextureCoords (false),
  m_dirty            (true)
{
    init();
}

Mesh::Mesh(const std::vector<Eigen::Vector3f> & vertData,
           const std::vector<Eigen::Vector3f> & normalData,
           const std::vector<Eigen::Vector2f> & textureCoordsData,
           const std::vector<Eigen::Vector3i> & faceData)
: m_vertData          (vertData),
  m_normalData        (normalData),
  m_textureCoordsData (textureCoordsData),
  m_faceData          (faceData),
  m_position          (Eigen::Vector3f::Zero()),
  m_VBOCreated        (false),
  m_hasTexture        (false),
  m_hasTextureCoords  (true),
  m_dirty             (true)
{
    init();
}

Mesh::Mesh(const Mesh & other)
    : m_vertData         (other.m_vertData),
      m_normalData       (other.m_normalData),
      m_colorData        (other.m_colorData),
      m_faceData         (other.m_faceData),
      m_position         (other.m_position),
      m_min              (other.m_min),
      m_max              (other.m_max),
      m_VBOCreated       (false),
      m_hasTexture       (other.m_hasTexture),
      m_hasTextureCoords (other.m_hasTextureCoords),
      m_dirty            (true)
{
    init();
}

void swap(Mesh & first, Mesh & second)
{
    using std::swap;

    swap(first.m_vertData,         second.m_vertData);
    swap(first.m_faceData,         second.m_faceData);
    swap(first.m_normalData,       second.m_normalData);
    swap(first.m_colorData,        second.m_colorData);

    swap(first.m_verts,            second.m_verts);
    swap(first.m_halfedges,        second.m_halfedges);
    swap(first.m_faces,            second.m_faces);

    swap(first.m_position,         second.m_position);

    swap(first.m_min,              second.m_min);
    swap(first.m_max,              second.m_max);

    swap(first.m_bvh,              second.m_bvh);

    swap(first.m_hasTexture,       second.m_hasTexture);
    swap(first.m_hasTextureCoords, second.m_hasTextureCoords);

    swap(first.m_dirty,            second.m_dirty);

    swap(first.m_vaoID,            second.m_vaoID);
    swap(first.m_indexVboID,       second.m_indexVboID);
    swap(first.m_posVboID,         second.m_posVboID);
    swap(first.m_normalVboID,      second.m_normalVboID);
    swap(first.m_colorVboID,       second.m_colorVboID);
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
    MH_ASSERT(m_vertData.size() == m_normalData.size());

    // set default mesh color
    m_colorData.resize(nVerts(), Eigen::Vector4f(0.75f, 0.75f, 0.75f, 1.0f));

    // create Vertex objects
    m_verts.reserve(nVerts());
    for (size_t i = 0; i < nVerts(); ++i)
    {
        m_verts.push_back(std::make_shared<Vertex>(
            m_vertData[i], m_normalData[i], m_colorData[i], m_textureCoordsData[i], i
        ));
    }

    // create Faces and HalfEdges
    m_faces.reserve(m_faceData.size());
    m_halfedges.reserve(3 * m_faceData.size());
    for (size_t i = 0; i < m_faceData.size(); ++i)
    {
        auto face = std::make_shared<Face>(i);
        m_faces.push_back(face);

        auto he_1 = std::make_shared<HalfEdge>();
        auto he_2 = std::make_shared<HalfEdge>();
        auto he_3 = std::make_shared<HalfEdge>();

        he_1->setVertex(m_verts[m_faceData[i][0]]);
        he_2->setVertex(m_verts[m_faceData[i][1]]);
        he_3->setVertex(m_verts[m_faceData[i][2]]);

        he_1->setFace(face);
        he_2->setFace(face);
        he_3->setFace(face);

        he_1->setNext(he_2);
        he_2->setNext(he_3);
        he_3->setNext(he_1);

        m_verts[m_faceData[i][2]]->setHalfEdge(he_1);
        m_verts[m_faceData[i][0]]->setHalfEdge(he_2);
        m_verts[m_faceData[i][1]]->setHalfEdge(he_3);

        face->setHalfEdge(he_1);

        m_halfedges.push_back(he_1);
        m_halfedges.push_back(he_2);
        m_halfedges.push_back(he_3);
    }

    // find twin edges
    std::map<std::pair<int, int>, std::shared_ptr<HalfEdge> > pairsVisited;
    for (size_t i = 0; i < m_halfedges.size(); ++i)
    {
        int idxA = m_halfedges[i]->getVertex()->idx();
        int idxB = m_halfedges[i]->getNext()->getVertex()->idx();
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

void Mesh::draw(void)
{
    update();

    glBindVertexArray(m_vaoID);
    glDrawElements(GL_TRIANGLES, 3 * nFaces(), GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}

void Mesh::createVBO(void)
{
    glGenVertexArrays(1, &m_vaoID);
    glBindVertexArray(m_vaoID);

    // array buffers
    MH_GEN_ARRAY_BUF(m_posVboID,    Eigen::Vector3f, nVerts(), &m_vertData[0](0),   GL_FLOAT, 3, POSITION_LOCATION);
    MH_GEN_ARRAY_BUF(m_normalVboID, Eigen::Vector3f, nVerts(), &m_normalData[0](0), GL_FLOAT, 3, NORMAL_LOCATION);
    MH_GEN_ARRAY_BUF(m_colorVboID,  Eigen::Vector4f, nVerts(), &m_colorData[0](0),  GL_FLOAT, 4, COLOR_LOCATION);

    if (m_hasTexture)
    {
        MH_GEN_ARRAY_BUF(m_textureVboID,  Eigen::Vector2f, nVerts(), &m_textureCoordsData[0](0), GL_FLOAT, 2, TEXTURE_LOCATION);
    }

    m_min = Eigen::Map<Eigen::Matrix<float, -1, 3, Eigen::RowMajor> >(&m_vertData[0](0), nVerts(), 3).colwise().minCoeff();
    m_max = Eigen::Map<Eigen::Matrix<float, -1, 3, Eigen::RowMajor> >(&m_vertData[0](0), nVerts(), 3).colwise().maxCoeff();

    // element array buffers
    std::vector<Eigen::Vector3i> fvi = getFVI();
    MH_GEN_ELEM_ARRAY_BUF(m_indexVboID, Eigen::Vector3i, nFaces(), &fvi[0](0));

    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

}

void Mesh::deleteVBO(void)
{
    glDeleteVertexArrays(1, &m_vaoID);
    glDeleteBuffers     (1, &m_indexVboID);
    glDeleteBuffers     (1, &m_posVboID);
    glDeleteBuffers     (1, &m_normalVboID);
    glDeleteBuffers     (1, &m_colorVboID);
    if (m_hasTexture)
    {
        glDeleteBuffers (1, &m_textureVboID);
    }
}

void Mesh::update(bool force)
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

Eigen::Vector3f Mesh::getCenter(void)
{
    update();
    return getModelToWorld() * ((m_min + m_max) / 2.0f);
}

Eigen::Affine3f Mesh::getModelToWorld(void) const
{
    return static_cast<Eigen::Affine3f>(Eigen::Translation3f(m_position));
}

std::shared_ptr<BVH> constructBVHFromMesh(Mesh * mesh)
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
