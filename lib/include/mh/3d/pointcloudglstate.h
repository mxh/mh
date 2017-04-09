#ifndef POINTCLOUDGLSTATE_H
#define POINTCLOUDGLSTATE_H

#include "mh/base/defs.h"
#include "mh/base/imports.h"

#include "mh/ext/gl3w/gl3w.h"

namespace mh
{

class Mesh;

class PointcloudGLState
{
public:
    static constexpr int POSITION_LOCATION   = 0;
    static constexpr int COLOR_LOCATION      = 2;
    static constexpr int INDEX_LOCATION      = 3;
    static constexpr int CUSTOM_INT_LOCATION = 4;
    static constexpr int CUSTOM_VEC_LOCATION = 5;

    PointcloudGLState(Mesh & mesh) : m_pointcloud(mesh), m_vboCreated(false) {}
    PointcloudGLState(PointcloudGLState & rhs)
        : m_pointcloud(rhs.m_pointcloud)
        , m_vboCreated(false) {}
    ~PointcloudGLState();

    void createVBO();
    void deleteVBO();

    void draw();

    Mesh & pointcloud() { return m_pointcloud; }

protected:

private:
    Mesh & m_pointcloud;

    bool   m_vboCreated;
    bool   m_hasTexture;

    GLuint m_vaoID;
    GLuint m_posVboID;
    GLuint m_colorVboID;
    GLuint m_indexVboID;
    GLuint m_customIntVboID;
    GLuint m_customVecVboID;

}; // class PointcloudGLState

struct PointcloudGLData
{
    std::vector<Eigen::Vector3f> vertData;
    std::vector<Eigen::Vector3f> colorData;
    std::vector<unsigned int>    indexData;

    std::vector<int>             customIntData;
    std::vector<Eigen::Vector3f> customVecData;
}; // struct PointcloudGLData

PointcloudGLData getPointcloudGLData(Mesh & pointcloud);

} // namespace mh

#endif
