#include "mh/3d/scene.h"

#include "mh/3d/camera.h"
#include "mh/3d/util.h"

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
    return m_center;
}

Eigen::Vector3f Scene::getMin(void) const
{
    Eigen::Vector3f min(0.0f, 0.0f, 0.0f);
    if (m_meshes.size())
    {
        min = m_meshes[0]->getMin();

        for (size_t i = 1; i < m_meshes.size(); ++i)
        {
            min = min.array().min(m_meshes[i]->getMin().array()).matrix();
        }
    }

    return min;
}

Eigen::Vector3f Scene::getMax(void) const
{
    Eigen::Vector3f max(0.0f, 0.0f, 0.0f);
    if (m_meshes.size())
    {
        max = m_meshes[0]->getMax();

        for (size_t i = 1; i < m_meshes.size(); ++i)
        {
            max = max.array().max(m_meshes[i]->getMax().array()).matrix();
        }
    }

    return max;
}

void Scene::draw(std::shared_ptr<Shader> shader, std::shared_ptr<CameraBase> camera)
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

void Scene::update(bool force)
{
    if (m_dirty || force)
    {
        std::vector<std::shared_ptr<BVH> > meshBVHs;
        //m_center = Eigen::Vector3f(0.0f, 0.0f, 0.0f);

        for (size_t i = 0; i < m_meshes.size(); ++i)
        {
        //    m_center += m_meshes[i]->getCenter();

            meshBVHs.push_back(m_meshes[i]->getBVH());
        }
        //m_center /= m_meshes.size();

        m_dirty = false;

        m_bvh = constructBVHFromSet(meshBVHs, BVH::X);
    }
}

void Scene::centerToCenterOfMass(void)
{
    m_center = Eigen::Vector3f(0.0f, 0.0f, 0.0f);

    for (size_t i = 0; i < m_meshes.size(); ++i)
    {
        m_center += m_meshes[i]->getCenter();
    }

    m_center /= m_meshes.size();
}

void Scene::reset(void)
{
    m_meshes.clear();

    m_bvh    = nullptr;
    m_center = Eigen::Vector3f(0.0f, 0.0f, 0.0f);
    m_dirty  = true;
}

float getSceneBoundingSphereRadius(Scene & scene)
{
    Eigen::Vector3f center = scene.getCenter();

    std::vector<float> radii;
    radii.resize(scene.getMeshes().size());
    std::transform(scene.getMeshes().begin(), scene.getMeshes().end(), radii.begin(),
        [&center] (const std::shared_ptr<Mesh> & x)
        {
            return getFittingSphereRadius(applyTransform(x->getModelToWorld(), x->getVertData()), center);
        }
    );
    
    return *std::max_element(radii.begin(), radii.end());
}

} // namespace mh
