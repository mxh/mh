#include "mh/3d/pointcloudglstate.h"
#include "mh/3d/mesh.h"

#include "mh/gpu/gpu_util.h"

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

namespace mh
{

PointcloudGLState::~PointcloudGLState()
{
    deleteVBO();
}

void PointcloudGLState::createVBO()
{
    glGenVertexArrays(1, &m_vaoID);
    glBindVertexArray(m_vaoID);

    PointcloudGLData pointcloudGLData = getPointcloudGLData(m_pointcloud);

    MH_GEN_ARRAY_BUF(m_posVboID,        Eigen::Vector3f, m_pointcloud.nVerts(), &pointcloudGLData.vertData[0](0),   GL_FLOAT,          3, POSITION_LOCATION);
    MH_GEN_ARRAY_BUF(m_colorVboID,      Eigen::Vector3f, m_pointcloud.nVerts(), &pointcloudGLData.colorData[0](0),  GL_FLOAT,          3, COLOR_LOCATION);
    MH_GEN_ARRAY_BUF_INT(m_indexVboID,  unsigned int,    m_pointcloud.nVerts(), &pointcloudGLData.indexData[0],     GL_UNSIGNED_INT,   1, INDEX_LOCATION);

    MH_GEN_ARRAY_BUF_INT (m_customIntVboID,  int,             m_pointcloud.nFaces() * 3, &pointcloudGLData.customIntData[0],    GL_UNSIGNED_INT, 1, CUSTOM_INT_LOCATION);
    MH_GEN_ARRAY_BUF     (m_customVecVboID,  Eigen::Vector3f, m_pointcloud.nFaces() * 3, &pointcloudGLData.customVecData[0](0), GL_FLOAT,        3, CUSTOM_VEC_LOCATION);

    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    m_vboCreated = true;
}

void PointcloudGLState::deleteVBO()
{
    if (!m_vboCreated) return;

    glDeleteVertexArrays(1, &m_vaoID);
    glDeleteBuffers     (1, &m_posVboID);
    glDeleteBuffers     (1, &m_colorVboID);
    glDeleteBuffers     (1, &m_indexVboID);
    glDeleteBuffers     (1, &m_customIntVboID);
    glDeleteBuffers     (1, &m_customVecVboID);

    m_vboCreated = false;
}

void PointcloudGLState::draw()
{
    glBindVertexArray(m_vaoID);
    glDrawArrays(GL_POINTS, 0, m_pointcloud.nVerts());
    glBindVertexArray(0);
}

PointcloudGLData getPointcloudGLData(Mesh & pointcloud)
{
    PointcloudGLData pointcloudGLData;

    pointcloudGLData.vertData.resize(pointcloud.nVerts());
    pointcloudGLData.colorData.resize(pointcloud.nVerts());
    pointcloudGLData.indexData.resize(pointcloud.nVerts());
    pointcloudGLData.customIntData.resize(pointcloud.nFaces() * 3);
    pointcloudGLData.customVecData.resize(pointcloud.nFaces() * 3);

    for (size_t i = 0; i < pointcloud.nVerts(); ++i)
    {
        pointcloudGLData.vertData[i]  = pointcloud.getVerts()[i]->getPosition();
        pointcloudGLData.colorData[i] = pointcloud.getVerts()[i]->getColor();
        pointcloudGLData.indexData[i] = pointcloud.getVerts()[i]->idx();
        pointcloudGLData.customIntData[i] = pointcloud.getVerts()[i]->getCustomInt();
        pointcloudGLData.customVecData[i] = pointcloud.getVerts()[i]->getCustomVec();
    }

    return pointcloudGLData;
}

} // namespace mh
