#include "mh/3d/scene.h"

#include "mh/3d/camera.h"

namespace mh
{

Scene::Scene(void)
{}

Scene::Scene(const std::vector<std::shared_ptr<Mesh> > & meshes)
    : m_meshes(meshes)
{}

void Scene::addMesh(std::shared_ptr<Mesh> mesh)
{
    m_meshes.push_back(mesh);
}

void Scene::addMeshes(const std::vector<std::shared_ptr<Mesh> > & meshes)
{
    m_meshes.reserve(m_meshes.size() + meshes.size());
    m_meshes.insert(m_meshes.end(), meshes.begin(), meshes.end());
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

void Scene::draw(std::shared_ptr<Shader> shader, std::shared_ptr<Camera> camera)
{
    shader->use();
    shader->setUniform("worldToCamera", camera->getWorldToCamera());
    shader->setUniform("cameraToClip",  camera->getCameraToClip());

    for (size_t i = 0; i < m_meshes.size(); ++i)
    {
        shader->setUniform("modelToWorld", transform_to_mtw(m_meshes[i]->getTransform()));

        shader->setUniform("diffuse", m_meshes[i]->getMaterial()->getDiffuse());
        shader->setUniform("specular", m_meshes[i]->getMaterial()->getSpecular());
        shader->setUniform("shininess", m_meshes[i]->getMaterial()->getShininess());

        m_meshes[i]->draw();
    }

    glUseProgram(0);
}

void Scene::centerToCenterOfMass(void)
{
    m_center = Eigen::Vector3f(0.0f, 0.0f, 0.0f);

    for (size_t i = 0; i < m_meshes.size(); ++i)
    {
        m_center += transform_to_mtw(m_meshes[i]->getTransform()) * m_meshes[i]->getCenter();
    }

    m_center /= m_meshes.size();
}

void Scene::reset(void)
{
    m_meshes.clear();
    m_center = Eigen::Vector3f(0.0f, 0.0f, 0.0f);
}

} // namespace mh
