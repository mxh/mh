#ifndef SCENE_H
#define SCENE_H 

#include "base/defs.h"
#include "base/imports.h"

#include "3d/mesh.h"

#include "gpu/shader.h"

namespace lp
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

} // namespace lp
#endif /* SCENE_H */
