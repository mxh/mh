#ifndef SCENE_H
#define SCENE_H 

#include "mh/base/defs.h"
#include "mh/base/imports.h"

#include "mh/3d/mesh.h"

#include "mh/gpu/shader.h"

namespace mh
{

class Camera;

class Scene
{
public:
    Scene  (void);
    Scene  (const std::vector<std::shared_ptr<Mesh> > & meshes);

    void                                        addMesh   (std::shared_ptr<Mesh> mesh);
    void                                        addMeshes (const std::vector<std::shared_ptr<Mesh> > & meshes);

    const std::vector<std::shared_ptr<Mesh> > & getMeshes (void) const { return m_meshes; }
          std::vector<std::shared_ptr<Mesh> > & getMeshes (void)       { return m_meshes; }

    void                                        draw      (std::shared_ptr<Shader> shader, std::shared_ptr<Camera> camera); 

    Eigen::Vector3f                             getCenter (void);

protected:

private:
    std::vector<std::shared_ptr<Mesh> > m_meshes;

    Eigen::Vector3f                     m_center;

    bool                                m_dirty;

}; // class Scene

} // namespace mh
#endif /* SCENE_H */
