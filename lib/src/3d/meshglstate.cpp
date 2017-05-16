#include "mh/3d/meshglstate.h"
#include "mh/3d/mesh.h"

#define MH_GEN_ARRAY_BUF(ID, TYPE, SIZE, START, GLTYPE, WIDTH, LOC) \
    glGenBuffers(1, &ID); \
    glBindBuffer(GL_ARRAY_BUFFER, ID); \
    glBufferData(GL_ARRAY_BUFFER, sizeof(TYPE) * SIZE, START, GL_STATIC_DRAW); \
    \
    glEnableVertexAttribArray(LOC);\
    glVertexAttribPointer(LOC, WIDTH, GLTYPE, 0, 0, 0)

#define MH_GEN_ARRAY_BUF_INT(ID, TYPE, SIZE, START, GLTYPE, WIDTH, LOC) \
    glGenBuffers(1, &ID); \
    glBindBuffer(GL_ARRAY_BUFFER, ID); \
    glBufferData(GL_ARRAY_BUFFER, sizeof(TYPE) * SIZE, START, GL_STATIC_DRAW); \
    \
    glEnableVertexAttribArray(LOC);\
    glVertexAttribIPointer(LOC, WIDTH, GLTYPE, 0, 0)

#define MH_GEN_ELEM_ARRAY_BUF(ID, TYPE, SIZE, START) \
    glGenBuffers(1, &ID); \
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ID); \
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(TYPE) * SIZE, START, GL_STATIC_DRAW)

namespace mh
{

MeshGLState::~MeshGLState()
{
    deleteVBO();
}

void MeshGLState::createVBO()
{
    glGenVertexArrays(1, &m_vaoID);
    glBindVertexArray(m_vaoID);

    MeshGLData meshGLData = getMeshGLData(m_mesh);

    MH_GEN_ARRAY_BUF     (m_posVboID,        Eigen::Vector3f, m_mesh.nFaces() * 3, &meshGLData.vertData[0](0),      GL_FLOAT,        3, POSITION_LOCATION);
    MH_GEN_ARRAY_BUF     (m_normalVboID,     Eigen::Vector3f, m_mesh.nFaces() * 3, &meshGLData.normalData[0](0),    GL_FLOAT,        3, NORMAL_LOCATION);
    MH_GEN_ARRAY_BUF     (m_colorVboID,      Eigen::Vector3f, m_mesh.nFaces() * 3, &meshGLData.colorData[0](0),     GL_FLOAT,        3, COLOR_LOCATION);
    MH_GEN_ARRAY_BUF_INT (m_indexVboID,      unsigned int,    m_mesh.nFaces() * 3, &meshGLData.indexData[0],        GL_UNSIGNED_INT, 1, INDEX_LOCATION);

    MH_GEN_ARRAY_BUF_INT (m_customIntVboID,  int,             m_mesh.nFaces() * 3, &meshGLData.customIntData[0],    GL_UNSIGNED_INT, 1, CUSTOM_INT_LOCATION);
    MH_GEN_ARRAY_BUF     (m_customVecVboID,  Eigen::Vector3f, m_mesh.nFaces() * 3, &meshGLData.customVecData[0](0), GL_FLOAT,        3, CUSTOM_VEC_LOCATION);

    if (m_mesh.hasTextureCoords() && m_mesh.getMaterial()->hasTexture())
    {
        MH_GEN_ARRAY_BUF(m_textureVboID, float2, m_mesh.nFaces() * 3, &meshGLData.textureCoordsData[0][0], GL_FLOAT, 2, TEXTURE_LOCATION);
        m_hasTexture = true;
    }

    MH_GEN_ELEM_ARRAY_BUF(m_faceVboID, Eigen::Vector3i, m_mesh.nFaces(), &meshGLData.faceData[0](0));

    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    m_vboCreated = true;
}

void MeshGLState::deleteVBO()
{
    if (!m_vboCreated) return;

    glDeleteVertexArrays(1, &m_vaoID);
    glDeleteBuffers     (1, &m_faceVboID);
    glDeleteBuffers     (1, &m_posVboID);
    glDeleteBuffers     (1, &m_normalVboID);
    glDeleteBuffers     (1, &m_colorVboID);
    glDeleteBuffers     (1, &m_indexVboID);
    glDeleteBuffers     (1, &m_customIntVboID);
    glDeleteBuffers     (1, &m_customVecVboID);

    if (m_hasTexture)
    {
        glDeleteBuffers(1, &m_textureVboID); 
    }

    m_vboCreated = false;
}

void MeshGLState::draw()
{
    glBindVertexArray(m_vaoID);
    glDrawElements(GL_TRIANGLES, 3 * m_mesh.nFaces(), GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}

MeshGLData getMeshGLData(const Mesh & mesh)
{
    MeshGLData meshGLData;

    // all vertices are duplicated such that each vertex belongs to
    // one face only, resulting in nFaces() * 3 vertices
    meshGLData.vertData.resize(mesh.nFaces() * 3);
    meshGLData.normalData.resize(mesh.nFaces() * 3);
    meshGLData.colorData.resize(mesh.nFaces() * 3);
    meshGLData.indexData.resize(mesh.nFaces() * 3);
    meshGLData.customIntData.resize(mesh.nFaces() * 3);
    meshGLData.customVecData.resize(mesh.nFaces() * 3);
    meshGLData.faceData.resize(mesh.nFaces());
    if (mesh.hasTextureCoords())
    {
        meshGLData.textureCoordsData.resize(mesh.nFaces() * 3);
    }

    // iterate over mesh faces, generate separate vertices for each face
    for (size_t i = 0; i < mesh.nFaces(); ++i)
    {
        meshGLData.vertData[i*3+0]   = mesh.getFaces()[i]->getVertex(0)->getPosition();
        meshGLData.vertData[i*3+1]   = mesh.getFaces()[i]->getVertex(1)->getPosition();
        meshGLData.vertData[i*3+2]   = mesh.getFaces()[i]->getVertex(2)->getPosition();

        meshGLData.normalData[i*3+0] = mesh.getFaces()[i]->getWedges()[0]->getNormal();
        meshGLData.normalData[i*3+1] = mesh.getFaces()[i]->getWedges()[1]->getNormal();
        meshGLData.normalData[i*3+2] = mesh.getFaces()[i]->getWedges()[2]->getNormal();

        meshGLData.colorData[i*3+0]  = mesh.getFaces()[i]->getVertex(0)->getColor();
        meshGLData.colorData[i*3+1]  = mesh.getFaces()[i]->getVertex(1)->getColor();
        meshGLData.colorData[i*3+2]  = mesh.getFaces()[i]->getVertex(2)->getColor();

        meshGLData.indexData[i*3+0] = mesh.getFaces()[i]->getVertex(0)->idx();
        meshGLData.indexData[i*3+1] = mesh.getFaces()[i]->getVertex(1)->idx();
        meshGLData.indexData[i*3+2] = mesh.getFaces()[i]->getVertex(2)->idx();

        meshGLData.customIntData[i*3+0] = mesh.getFaces()[i]->getVertex(0)->getCustomInt();
        meshGLData.customIntData[i*3+1] = mesh.getFaces()[i]->getVertex(1)->getCustomInt();
        meshGLData.customIntData[i*3+2] = mesh.getFaces()[i]->getVertex(2)->getCustomInt();

        meshGLData.customVecData[i*3+0] = mesh.getFaces()[i]->getVertex(0)->getCustomVec();
        meshGLData.customVecData[i*3+1] = mesh.getFaces()[i]->getVertex(1)->getCustomVec();
        meshGLData.customVecData[i*3+2] = mesh.getFaces()[i]->getVertex(2)->getCustomVec();

        if (mesh.hasTextureCoords())
        {
            meshGLData.textureCoordsData[i*3+0] = mesh.getFaces()[i]->getWedges()[0]->getTextureCoords();
            meshGLData.textureCoordsData[i*3+1] = mesh.getFaces()[i]->getWedges()[1]->getTextureCoords();
            meshGLData.textureCoordsData[i*3+2] = mesh.getFaces()[i]->getWedges()[2]->getTextureCoords();
        }

        meshGLData.faceData[i] = {static_cast<int>(i)*3+0,
                                  static_cast<int>(i)*3+1, 
                                  static_cast<int>(i)*3+2};
    }

    return meshGLData;
}

} // namespace mh
