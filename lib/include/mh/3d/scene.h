#ifndef SCENE_H
#define SCENE_H 

#include "mh/base/defs.h"
#include "mh/base/imports.h"

#include "mh/3d/bvh.h"
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

    void                                        addMesh              (std::shared_ptr<Mesh> mesh);
    void                                        addMeshes            (const std::vector<std::shared_ptr<Mesh> > & meshes);

    const std::vector<std::shared_ptr<Mesh> > & getMeshes            (void) const { return m_meshes; }
          std::vector<std::shared_ptr<Mesh> > & getMeshes            (void)       { return m_meshes; m_dirty = true;}

    std::shared_ptr<BVH>                        getBVH               (void)       { return m_bvh; }

    void                                        draw                 (std::shared_ptr<Shader> shader, std::shared_ptr<Camera> camera); 

    Eigen::Vector3f                             getCenter            (void);

    Eigen::Vector3f                             getMin               (void) const;
    Eigen::Vector3f                             getMax               (void) const;

    void                                        update               (bool force=false);
    void                                        centerToCenterOfMass (void);

    void                                        reset                (void);

protected:

private:
    // meshes
    std::vector<std::shared_ptr<Mesh> > m_meshes;

    // bounding volume hierarchy
    std::shared_ptr<BVH>                m_bvh;

    Eigen::Vector3f                     m_center;

    bool                                m_dirty;

}; // class Scene

float getSceneBoundingSphereRadius(Scene & scene);

} // namespace mh
#endif /* SCENE_H */
