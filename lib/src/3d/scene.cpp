#include "mh/3d/scene.h"

#include "mh/3d/camera.h"

namespace mh
{

Scene::Scene(void) : m_dirty(true)
{
    
}

Scene::Scene(const std::vector<std::shared_ptr<Mesh> > & meshes)
    : m_meshes(meshes), m_dirty(true)
{

}

void Scene::addMesh(std::shared_ptr<Mesh> mesh)
{
    m_meshes.push_back(mesh);
    m_dirty = true;
}

void Scene::addMeshes(const std::vector<std::shared_ptr<Mesh> > & meshes)
{
    m_meshes.reserve(m_meshes.size() + meshes.size());
    m_meshes.insert(m_meshes.end(), meshes.begin(), meshes.end());
    m_dirty = true;
}

Eigen::Vector3f Scene::getCenter(void)
{
    update();

    return m_center;
}

void Scene::draw(std::shared_ptr<Shader> shader, std::shared_ptr<Camera> camera)
{
    update();

    shader->use();
    shader->setUniform("worldToCamera", camera->getWorldToCamera());
    shader->setUniform("cameraToClip",  camera->getCameraToClip());

    for (size_t i = 0; i < m_meshes.size(); ++i)
    {
        shader->setUniform("modelToWorld", m_meshes[i]->getModelToWorld());
        m_meshes[i]->draw();
    }

    glUseProgram(0);
}

void Scene::update(void)
{
    if (m_dirty)
    {
        std::vector<std::shared_ptr<BVH> > meshBVHs;
        m_center = Eigen::Vector3f(0.0f, 0.0f, 0.0f);

        for (size_t i = 0; i < m_meshes.size(); ++i)
        {
            m_center += m_meshes[i]->getCenter();

            meshBVHs.push_back(m_meshes[i]->getBVH());
        }
        m_center /= m_meshes.size();

        m_dirty = false;

        m_bvh = constructBVHFromSet(meshBVHs, BVH::X);
    }
}

void Scene::reset(void)
{
    m_meshes.clear();

    m_bvh    = nullptr;
    m_center = Eigen::Vector3f(0.0f, 0.0f, 0.0f);
    m_dirty  = true;
}

} // namespace mh
