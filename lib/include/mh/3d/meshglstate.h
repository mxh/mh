#ifndef MESHGLSTATE_H
#define MESHGLSTATE_H

#include "mh/base/defs.h"
#include "mh/base/imports.h"

#include "mh/ext/gl3w/gl3w.h"

namespace mh
{

class Mesh;

class MeshGLState
{
public:
    static constexpr int POSITION_LOCATION   = 0;
    static constexpr int NORMAL_LOCATION     = 1;
    static constexpr int COLOR_LOCATION      = 2;
    static constexpr int INDEX_LOCATION      = 3;
    static constexpr int CUSTOM_INT_LOCATION = 4;
    static constexpr int CUSTOM_VEC_LOCATION = 5;
    static constexpr int TEXTURE_LOCATION    = 6;

    MeshGLState(Mesh & mesh) : m_mesh(mesh), m_vboCreated(false) {}
    MeshGLState(MeshGLState & rhs)
        : m_mesh(rhs.m_mesh)
        , m_vboCreated(false) {}
    ~MeshGLState();

    void createVBO();
    void deleteVBO();

    void draw();

    Mesh & mesh() { return m_mesh; }

protected:

private:
    Mesh & m_mesh;

    bool   m_vboCreated;
    bool   m_hasTexture;

    GLuint m_vaoID;
    GLuint m_faceVboID;
    GLuint m_posVboID;
    GLuint m_normalVboID;
    GLuint m_colorVboID;
    GLuint m_indexVboID;
    GLuint m_customIntVboID;
    GLuint m_customVecVboID;
    GLuint m_textureVboID;

}; // class MeshGLState

struct MeshGLData
{
    std::vector<Eigen::Vector3f> vertData;
    std::vector<Eigen::Vector3f> normalData;
    std::vector<Eigen::Vector3f> colorData;
    std::vector<unsigned int>    indexData;
    std::vector<int>             customIntData;
    std::vector<Eigen::Vector3f> customVecData;

    std::vector<Eigen::Vector3i> faceData;

    std::vector<float2>          textureCoordsData;
}; // struct MeshGLData

MeshGLData getMeshGLData(const Mesh & mesh);

} // namespace mh

#endif
