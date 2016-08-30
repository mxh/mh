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
    static constexpr int POSITION_LOCATION = 0;
    static constexpr int NORMAL_LOCATION   = 1;
    static constexpr int TEXTURE_LOCATION  = 2;

    MeshGLState(Mesh & mesh) : m_mesh(mesh) {}

    void createVBO();
    void deleteVBO();

    void draw();

protected:

private:
    Mesh & m_mesh;

    bool   m_vboCreated;

    GLuint m_vaoID;
    GLuint m_faceVboID;
    GLuint m_posVboID;
    GLuint m_normalVboID;
    GLuint m_textureVboID;

}; // class MeshGLState

struct MeshGLData
{
    std::vector<Eigen::Vector3f> vertData;
    std::vector<Eigen::Vector3f> normalData;

    std::vector<Eigen::Vector3i> faceData;

    std::vector<float2>          textureCoordsData;
}; // struct MeshGLData

MeshGLData getMeshGLData(Mesh & mesh);

} // namespace mh

#endif
